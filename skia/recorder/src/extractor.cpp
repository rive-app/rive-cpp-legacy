#include "extractor.hpp"

inline int valueOrDefault(int value, int default_value)
{
	return value <= 0 ? default_value : value;
}

inline void scale(int* value, int targetValue, int* otherValue)
{
	if (*value != targetValue)
	{
		*otherValue = *otherValue * targetValue / *value;
		*value = targetValue;
	}
}

RiveFrameExtractor::RiveFrameExtractor(const char* path,
                                       const char* artboard_name,
                                       const char* animation_name,
                                       const char* watermark_name,
                                       int width,
                                       int height,
                                       int small_extent_target,
                                       int max_width,
                                       int max_height,
                                       int min_duration,
                                       int max_duration,
                                       float fps)
{
	m_MinDuration = min_duration;
	m_MaxDuration = max_duration;
	m_RiveFile = getRiveFile(path);
	m_Artboard = getArtboard(artboard_name);
	m_Animation = getAnimation(animation_name);
	m_Animation_instance = new rive::LinearAnimationInstance(m_Animation);
	m_WatermarkImage = getWatermark(watermark_name);
	initializeDimensions(
	    width, height, small_extent_target, max_width, max_height);
	m_RasterSurface = SkSurface::MakeRaster(SkImageInfo::Make(
	    m_Width, m_Height, kRGBA_8888_SkColorType, kPremul_SkAlphaType));
	m_RasterCanvas = m_RasterSurface->getCanvas();
	m_Fps = valueOrDefault(fps, m_Animation->fps());
	ifps = 1.0 / m_Fps;
};

RiveFrameExtractor::~RiveFrameExtractor()
{
	if (m_Animation_instance)
	{
		delete m_Animation_instance;
	}
	if (m_RiveFile)
	{
		delete m_RiveFile;
	}
}

int RiveFrameExtractor::width() const { return m_Width; };
int RiveFrameExtractor::height() const { return m_Height; };
float RiveFrameExtractor::fps() const { return m_Fps; };
int RiveFrameExtractor::totalFrames() const
{
	int min_frames = m_MinDuration * fps();
	int max_frames = m_MaxDuration * fps();

	int startFrame =
	    m_Animation->enableWorkArea() ? m_Animation->workStart() : 0;
	int endFrame = m_Animation->enableWorkArea() ? m_Animation->workEnd()
	                                             : m_Animation->duration();
	int animationFrames = endFrame - startFrame;
	int totalFrames = animationFrames;

	// TODO: combine those two into one function
	switch (m_Animation->loop())
	{
		case rive::Loop::pingPong:
			animationFrames *= 2;
		// No break: falls through.
		case rive::Loop::loop:
			// pingpong is like loop, just you gotta go back and forth, so 2x
			// duration
			if (min_frames != 0 && totalFrames < min_frames)
			{
				totalFrames = std::ceil(min_frames / float(animationFrames)) *
				              animationFrames;
			}
			if (max_frames != 0 && totalFrames > max_frames &&
			    animationFrames < max_frames)
			{
				totalFrames = std::floor(max_frames / (animationFrames)) *
				              animationFrames;
			}
			break;
		default:
			break;
	}
	// no matter what shenanigans we had before, lets make sure we fall
	// in line regardless.
	if (min_frames != 0 && totalFrames < min_frames)
	{
		totalFrames = min_frames;
	}
	if (max_frames != 0 && totalFrames > max_frames)
	{
		totalFrames = max_frames;
	}
	return totalFrames;
};

void RiveFrameExtractor::initializeDimensions(int width,
                                              int height,
                                              int small_extent_target,
                                              int max_width,
                                              int max_height)
{
	// Take the width & height from user input, or from the provided artboard
	m_Width = valueOrDefault(width, m_Artboard->width());
	m_Height = valueOrDefault(height, m_Artboard->height());

	// if we have a target value for whichever extent is smaller, lets scale to
	// that.
	if (small_extent_target != 0)
	{
		if (m_Width < m_Height)
		{
			scale(&m_Width, small_extent_target, &m_Height);
		}
		else
		{
			scale(&m_Height, small_extent_target, &m_Width);
		}
	}

	// if we have a max height, lets scale down to that
	if (max_height != 0 && max_height < m_Height)
	{
		scale(&m_Height, max_height, &m_Width);
	}

	// if we have a max width, lets scale down to that
	if (max_width != 0 && max_width < m_Width)
	{
		scale(&m_Width, max_width, &m_Height);
	}

	// We're sticking with 2's right now. so you know it is what it is.
	m_Width = nextMultipleOf(m_Width, 2);
	m_Height = nextMultipleOf(m_Height, 2);
}

sk_sp<SkImage>
RiveFrameExtractor::getWatermark(const char* watermark_name) const
{
	// Init skia surfaces to render to.
	sk_sp<SkImage> watermarkImage;
	if (watermark_name != NULL && watermark_name[0] != '\0')
	{

		if (!file_exists(watermark_name))
		{
			throw std::invalid_argument(string_format(
			    "Cannot find file containing watermark at %s", watermark_name));
		}
		if (auto data = SkData::MakeFromFileName(watermark_name))
		{
			watermarkImage = SkImage::MakeFromEncoded(data);
		}
	}
	return watermarkImage;
}

rive::File* RiveFrameExtractor::getRiveFile(const char* path) const
{
	FILE* fp = fopen(path, "r");

	if (fp == nullptr)
	{
		fclose(fp);
		throw std::invalid_argument(
		    string_format("Failed to open file %s", path));
	}
	fseek(fp, 0, SEEK_END);
	auto length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t* bytes = new uint8_t[length];

	if (fread(bytes, 1, length, fp) != length)
	{
		fclose(fp);
		delete[] bytes;
		throw std::invalid_argument(
		    string_format("Failed to read file into bytes array %s", path));
	}

	auto reader = rive::BinaryReader(bytes, length);
	rive::File* file = nullptr;
	auto result = rive::File::import(reader, &file);

	fclose(fp);
	delete[] bytes;

	if (result != rive::ImportResult::success)
	{
		throw std::invalid_argument(
		    string_format("Failed to read bytes into Rive file %s", path));
	}
	return file;
}

rive::Artboard* RiveFrameExtractor::getArtboard(const char* artboard_name) const
{
	// Figure out which artboard to use.
	rive::Artboard* artboard;

	// QUESTION: better to keep this logic in the main? or pass the flag
	// in here, so we can bool check if the flag is set or not? what
	// happens if we try to target the artboard '' otherwise?
	//
	if (artboard_name != NULL && artboard_name[0] != '\0')
	{
		if ((artboard = m_RiveFile->artboard(artboard_name)) == nullptr)
		{
			throw std::invalid_argument(string_format(
			    "File doesn't contain an artboard named %s.", artboard_name));
		}
	}
	else
	{
		artboard = m_RiveFile->artboard();
		if (artboard == nullptr)
		{
			throw std::invalid_argument(string_format(
			    "File doesn't contain a default artboard.", artboard_name));
		}
	}
	return artboard;
}

rive::LinearAnimation*
RiveFrameExtractor::getAnimation(const char* animation_name) const
{
	// Figure out which animation to use.
	rive::LinearAnimation* animation;
	if (animation_name != NULL && animation_name[0] != '\0')
	{
		if ((animation = m_Artboard->animation(animation_name)) == nullptr)
		{

			fprintf(stderr,
			        "Artboard doesn't contain an animation named %s.\n",
			        animation_name);
		}
	}
	else
	{
		animation = m_Artboard->firstAnimation();
		if (animation == nullptr)
		{
			throw std::invalid_argument(
			    string_format("Artboard doesn't contain a default animation."));
		}
	}
	return animation;
};

void RiveFrameExtractor::advanceFrame() const
{
	m_Animation_instance->advance(ifps);
}

sk_sp<SkImage> RiveFrameExtractor::getSnapshot() const
{

	// I see a canvas and I want to paint it black.
	// (without this transparent background dont get cleared.)
	SkPaint paint;
	m_RasterCanvas->clear(SK_ColorBLACK);

	// hmm "no deafault constructor exists bla bla... "
	rive::SkiaRenderer renderer(m_RasterCanvas);

	renderer.save();
	renderer.align(rive::Fit::cover,
	               rive::Alignment::center,
	               rive::AABB(0, 0, width(), height()),
	               m_Artboard->bounds());
	m_Animation_instance->apply(m_Artboard);
	m_Artboard->advance(0.0f);
	m_Artboard->draw(&renderer);
	renderer.restore();
	if (m_WatermarkImage)
	{
		SkPaint watermarkPaint;
		watermarkPaint.setBlendMode(SkBlendMode::kDifference);
		m_RasterCanvas->drawImage(m_WatermarkImage,
		                          width() - m_WatermarkImage->width() - 50,
		                          height() - m_WatermarkImage->height() - 50,
		                          &watermarkPaint);
	}

	// After drawing the frame, grab the raw image data.
	sk_sp<SkImage> img(m_RasterSurface->makeImageSnapshot());
	if (!img)
	{
		throw std::invalid_argument(string_format("Cant make a snapshot."));
	}
	return img;
}

const void* RiveFrameExtractor::getPixelAddresses() const
{
	auto img = getSnapshot();
	SkPixmap pixels;
	if (!img->peekPixels(&pixels))
	{
		throw std::invalid_argument(
		    string_format("Cant peek pixels image frame from riv file."));
	}

	// Get the address to the first pixel (addr8 will assert in debug mode
	// as Skia only wants you to use that with 8 bit surfaces).
	return pixels.addr(0, 0);
};

sk_sp<SkData> RiveFrameExtractor::getSkData() const
{
	auto img = getSnapshot();
	sk_sp<SkData> png(img->encodeToData());
	if (!png)
	{
		throw std::invalid_argument(
		    string_format("Cant encode snapshot as png."));
	}

	// Get the address to the first pixel (addr8 will assert in debug mode
	// as Skia only wants you to use that with 8 bit surfaces).
	return png;
};