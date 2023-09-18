#pragma once

#include "SOP_Triangulations_Sphere.proto.h"

#include "SOP_Triangulations_Sphere_Info.hpp"
#include <GA/GA_PolyCounts.h>
#include <GEO/GEO_PrimPoly.h>
#include <GU/GU_Detail.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_AutoLockInputs.h>
#include <PRM/PRM_Include.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
#include <limits.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_on_sphere_2.h>
#include <CGAL/Projection_on_sphere_traits_3.h>

#include <vector>
#include <unordered_map>

#include <fstream>

class SOP_Triangulations_Sphere_Verb : public SOP_NodeVerb
{
public:
    inline static const SOP_NodeVerb::Register<SOP_Triangulations_Sphere_Verb> theVerb;

    SOP_Triangulations_Sphere_Verb() {}
    virtual ~SOP_Triangulations_Sphere_Verb() {}

    virtual SOP_NodeParms *allocParms() const
    {
        return new SOP_Triangulations_SphereParms();
    }

    virtual UT_StringHolder name() const 
    {
        return SOP_Triangulations_Sphere_Info::TYPENAME;
    }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const 
    {
        return COOK_GENERIC; 
    }

private:
    using Kernel = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;    
    using FT = Kernel::FT;
    using Traits = CGAL::Projection_on_sphere_traits_3<Kernel>;
    using CGALPoint_3 = Traits::Point_3;
    using Triangulation = CGAL::Delaunay_triangulation_on_sphere_2<Traits>;

    auto makeFace(GU_Detail* detail, Triangulation&& triangulation) const
    {
        std::unordered_map<Triangulation::Vertex_handle, GA_Offset> indexMap;

        auto&& startptoff = detail->appendPointBlock(triangulation.number_of_vertices());
        exint&& point_idx = 0;
        for (auto&& v = triangulation.vertices_begin(); v != triangulation.vertices_end(); ++v)
        {
            auto&& ptoff = startptoff + point_idx;
            auto&& pos = UT_Vector3D{
                    CGAL::to_double(v->point().x()),
                    CGAL::to_double(v->point().y()),
                    CGAL::to_double(v->point().z())
                };

            detail->setPos3(ptoff, pos);

            indexMap[v] = static_cast<GA_Offset>(ptoff);

            point_idx++;
        }
        
        std::vector<int> indices;
        for (auto&& f = triangulation.finite_faces_begin(); f != triangulation.finite_faces_end(); ++f)
        {
            indices.push_back(indexMap[f->vertex(0)]);
            indices.push_back(indexMap[f->vertex(1)]);
            indices.push_back(indexMap[f->vertex(2)]);
        }

        GA_PolyCounts polyCounts;
        polyCounts.append(3, triangulation.number_of_faces());
        GEO_PrimPoly::buildBlock(
                                    detail,
                                    static_cast<GA_Offset>(0),
                                    static_cast<GA_Size>(triangulation.number_of_vertices()),
                                    polyCounts,
                                    indices.data()
                                );
    }

public:
    virtual void cook(const CookParms &cookparms) const
    {
        auto&& inputGeo = cookparms.inputGeo(0);
        auto&& outputGeo = cookparms.gdh().gdpNC();
        if (!inputGeo)
            return;
        
        outputGeo->clearAndDestroy();

        std::vector<CGALPoint_3> points(inputGeo->getNumPoints());
        unsigned int&& index = 0;
        for(auto&& point : points)
        {
            auto&& pointOffset = inputGeo->pointOffset(index);
            auto&& pointPosition = inputGeo->getPos3D(pointOffset);

            point = {
                    pointPosition.x(),
                    pointPosition.y(),
                    pointPosition.z()
                };

            index++;
        }

        const int&& radius = 1; // TODO: パラメータ化する
        auto&& center = CGALPoint_3(0, 0, 0);
        auto&& traits = Traits(center, radius);
        auto&& dtos = Triangulation(points.begin(), points.end(), traits);

        this->makeFace(outputGeo, std::move(dtos));
    }

};
