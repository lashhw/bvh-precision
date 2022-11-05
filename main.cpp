#include <iostream>
#include <mpfr.h>
#include "bvh-analysis/utils/parse.hpp"
#include "bvh-analysis/utils/build_bvh.hpp"
#include "bvh/single_ray_traverser.hpp"
#include "bvh/primitive_intersectors.hpp"

using Ray = bvh::Ray<float>;
using PrimitiveIntersector = bvh::ClosestPrimitiveIntersector<Bvh, Triangle>;
using Traverser = bvh::SingleRayTraverser<Bvh>;
using MPTraverser = bvh::SingleRayTraverser<Bvh, 64, bvh::MPNodeIntersector<Bvh, 24, 8>>;

int main() {
    std::vector<Triangle> triangles = parse_ply("sponza.ply");
    Bvh bvh = build_bvh(triangles);
    std::cout << "BVH has " << bvh.node_count << " nodes" << std::endl;

    PrimitiveIntersector primitive_intersector(bvh, triangles.data());
    Traverser traverser(bvh);
    MPTraverser mp_traverser(bvh);

    std::ifstream ray_queries_file("ray_queries.bin", std::ios::in | std::ios::binary);
    float r[7];
    while (ray_queries_file.read(reinterpret_cast<char*>(&r), 7 * sizeof(float))) {
        Ray ray(
                Vector3(r[0], r[1], r[2]),
                Vector3(r[3], r[4], r[5]),
                0.f,
                r[6]
        );

        MPTraverser::Statistics statistics;
        auto result = mp_traverser.traverse(ray, primitive_intersector, statistics);

        Traverser::Statistics ref_statistics;
        auto ref_result = traverser.traverse(ray, primitive_intersector, ref_statistics);

        std::cout << statistics.traversal_steps << ", "
                  << ref_statistics.traversal_steps << std::endl;
        std::cout << statistics.intersections << ", "
                  << ref_statistics.intersections << std::endl;

        /*
        if (result->intersection.t != ref_result->intersection.t) {
            std::cerr << result->intersection.t << ", "
                      << ref_result->intersection.t << std::endl;
            exit(EXIT_FAILURE);
        }
         */
    }
}
