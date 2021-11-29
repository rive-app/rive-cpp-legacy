#ifndef _RIVE_RELATIVE_LOCAL_ASSET_RESOLVER_HPP_
#define _RIVE_RELATIVE_LOCAL_ASSET_RESOLVER_HPP_

#include "rive/file_asset_resolver.hpp"
#include "rive/assets/file_asset.hpp"
#include <cstdio>
#include <string>

namespace rive
{
	class FileAsset;
	/// An implementation of FileAssetResolver which finds the assets in a local
	/// path relative to the original .riv file looking for them.
	class RelativeLocalAssetResolver : public FileAssetResolver
	{
	private:
		std::string m_Path;

	public:
		RelativeLocalAssetResolver(std::string filename)
		{
			std::size_t finalSlash = filename.rfind('/');

			if (finalSlash != std::string::npos)
			{
				m_Path = filename.substr(0, finalSlash + 1);
			}
		}

		void loadContents(FileAsset& asset) override
		{
			std::string filename = m_Path + asset.uniqueFilename();
			FILE* fp = fopen(filename.c_str(), "r");

			fseek(fp, 0, SEEK_END);
			auto length = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			uint8_t* bytes = new uint8_t[length];
			if (fread(bytes, 1, length, fp) == length)
			{
				asset.decode(bytes, length);
			}
			delete[] bytes;
		}
	};
} // namespace rive
#endif
