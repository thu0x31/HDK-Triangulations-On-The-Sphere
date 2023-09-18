#include "SOP_Triangulations_Sphere.hpp"
#include "SOP_Triangulations_Sphere_Info.hpp"

#include <PRM/PRM_TemplateBuilder.h>

constexpr const char* DSFILE = R"THEDSFILE(
{
    name        parameters
    parm {
        name    "divs"      // Internal parameter name
        label   "Divisions" // Descriptive parameter name for user interface
        type    integer
        default { "5" }     // Default for this parameter on new nodes
        range   { 2! 50 }   // The value is prevented from going below 2 at all.
                            // The UI slider goes up to 50, but the value can go higher.
        export  all         // This makes the parameter show up in the toolbox
                            // above the viewport when it's in the node's state.
    }
    parm {
        name    "rad"
        label   "Radius"
        type    vector2
        size    2           // 2 components in a vector2
        default { "1" "0.3" } // Outside and inside radius defaults
    }
    parm {
        name    "nradius"
        label   "Allow Negative Radius"
        type    toggle
        default { "0" }
    }
    parm {
        name    "t"
        label   "Center"
        type    vector
        size    3           // 3 components in a vector
        default { "0" "0" "0" }
    }
    parm {
        name    "orient"
        label   "Orientation"
        type    ordinal
        default { "0" }     // Default to first entry in menu, "xy"
        menu    {
            "xy"    "XY Plane"
            "yz"    "YZ Plane"
            "zx"    "ZX Plane"
        }
    }
}
)THEDSFILE";

PRM_Template* SOP_Triangulations_Sphere::buildTemplates()
{
    static PRM_TemplateBuilder templ(
            "SOP_Triangulations_Sphere.cpp"_sh,
            DSFILE
        );
    return templ.templates();
}

void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        SOP_Triangulations_Sphere_Info::TYPENAME,   // Internal name
        SOP_Triangulations_Sphere_Info::NAME,                     // UI name
        SOP_Triangulations_Sphere::myConstructor,    // How to build the SOP
        SOP_Triangulations_Sphere::buildTemplates(), // My parameters
        1,                          // Min # of sources
        1,                          // Max # of sources
        nullptr,                    // Custom local variables (none)
        OP_FLAG_GENERATOR));        // Flag it as generator
}
