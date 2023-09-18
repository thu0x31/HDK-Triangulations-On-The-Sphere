#pragma once

#include "SOP_Triangulations_On_The_Sphere_Verb.hpp"

#include <SOP/SOP_Node.h>

class SOP_Triangulations_On_The_Sphere : public SOP_Node
{
public:
    static PRM_Template *buildTemplates();
    static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op)
    {
        return new SOP_Triangulations_On_The_Sphere(net, name, op);
    }
    const SOP_NodeVerb *cookVerb() const override
    { 
        return SOP_Triangulations_On_The_Sphere_Verb::theVerb.get();
    }
    
protected:
    SOP_Triangulations_On_The_Sphere(OP_Network *net, const char *name, OP_Operator *op)
        : SOP_Node(net, name, op)
    {
        // All verb SOPs must manage data IDs, to track what's changed
        // from cook to cook.
        mySopFlags.setManagesDataIDs(true);
    }
    ~SOP_Triangulations_On_The_Sphere() override {}
    /// Since this SOP implements a verb, cookMySop just delegates to the verb.
    OP_ERROR cookMySop(OP_Context &context) override
    {
        return cookMyselfAsVerb(context);
    }
};