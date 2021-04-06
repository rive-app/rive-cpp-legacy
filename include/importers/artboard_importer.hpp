#ifndef _RIVE_ARTBOARD_IMPORTER_HPP_
#define _RIVE_ARTBOARD_IMPORTER_HPP_

#include "importers/import_stack.hpp"

namespace rive
{
    class Core;
    class Artboard;
    class LinearAnimation;
    class StateMachine;
	class ArtboardImporter : public ImportStackObject
	{
	private:
		Artboard* m_Artboard;

	public:
		ArtboardImporter(Artboard* artboard);
        void addComponent(Core* object);
        void addAnimation(LinearAnimation* animation);
        void addStateMachine(StateMachine* stateMachine);
		StatusCode resolve() override;
	};
} // namespace rive
#endif
