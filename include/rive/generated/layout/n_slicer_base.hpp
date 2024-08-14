#ifndef _RIVE_N_SLICER_BASE_HPP_
#define _RIVE_N_SLICER_BASE_HPP_
#include "rive/component.hpp"
namespace rive
{
class NSlicerBase : public Component
{
protected:
    typedef Component Super;

public:
    static const uint16_t typeKey = 493;

    /// Helper to quickly determine if a core object extends another without RTTI
    /// at runtime.
    bool isTypeOf(uint16_t typeKey) const override
    {
        switch (typeKey)
        {
            case NSlicerBase::typeKey:
            case ComponentBase::typeKey:
                return true;
            default:
                return false;
        }
    }

    uint16_t coreType() const override { return typeKey; }

    Core* clone() const override;

protected:
};
} // namespace rive

#endif