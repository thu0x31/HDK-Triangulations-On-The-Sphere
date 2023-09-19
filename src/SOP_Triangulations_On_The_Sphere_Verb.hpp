#pragma once

#include "SOP_Triangulations_On_The_Sphere.proto.h"

#include "SOP_Triangulations_On_The_Sphere_Info.hpp"
#include <GA/GA_Detail.h>
#include <GA/GA_PolyCounts.h>
#include <GEO/GEO_PrimPoly.h>
#include <GEO/GEO_Point.h>
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

class SOP_Triangulations_On_The_Sphere_Verb : public SOP_NodeVerb
{
public:
    inline static const SOP_NodeVerb::Register<SOP_Triangulations_On_The_Sphere_Verb> theVerb;

    SOP_Triangulations_On_The_Sphere_Verb() {}
    virtual ~SOP_Triangulations_On_The_Sphere_Verb() {}

    virtual SOP_NodeParms *allocParms() const
    {
        return new SOP_Triangulations_On_The_SphereParms();
    }

    virtual UT_StringHolder name() const 
    {
        return SOP_Triangulations_On_The_Sphere_Info::TYPENAME;
    }

    virtual CookMode cookMode(const SOP_NodeParms *parms) const 
    {
        return COOK_GENERIC; 
    }

private:
    using Kernel = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;    
    using FT = Kernel::FT;
    using Traits = CGAL::Projection_on_sphere_traits_3<Kernel>;
    using Triangulation = CGAL::Delaunay_triangulation_on_sphere_2<Traits>;

    auto makePoly(GU_Detail* detail, Triangulation&& triangulation) const
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
        
        constexpr auto POLYSIZE = 3;
        std::vector<int> indices(triangulation.number_of_faces() * POLYSIZE);
        int index = 0;
        for (auto&& f = triangulation.finite_faces_begin(); f != triangulation.finite_faces_end(); ++f)
        {
            indices[index]     = indexMap[f->vertex(0)];
            indices[index + 1] = indexMap[f->vertex(1)];
            indices[index + 2] = indexMap[f->vertex(2)];

            index += POLYSIZE;
        }

        GA_PolyCounts polyCounts;
        polyCounts.append(POLYSIZE, triangulation.number_of_faces());
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
        if (!inputGeo)
            return;
        
        auto&& outputGeo = cookparms.gdh().gdpNC();
        outputGeo->clearAndDestroy();

        std::cout << "input geo point num : " << inputGeo->getNumPoints() << std::endl;

        std::vector<Traits::Point_3> points(inputGeo->getNumPoints());
        inputGeo->forEachPoint(
            [&points, &inputGeo] (GA_Offset ptoff) 
            {
                auto&& pos = inputGeo->getPos3D(ptoff);
                points[ptoff] = { pos.x(), pos.y(), pos.z() };
            }
        );

        const int&& radius = 1; // TODO: パラメータ化する
        auto&& dtos = Triangulation(points.begin(), points.end(), Traits({0,0,0}, radius));

        std::cout << "triangulation point num : " << dtos.number_of_vertices() << std::endl;

        this->makePoly(outputGeo, std::move(dtos));

        std::cout << "output geo point num : " << outputGeo->getNumPoints() << std::endl;
    }

};
