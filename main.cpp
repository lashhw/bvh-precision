#include <iostream>
#include <mpfr.h>
#include "bvh-analysis/utils/parse.hpp"
#include "bvh-analysis/utils/build_bvh.hpp"
#include "bvh/single_ray_traverser.hpp"
#include "bvh/primitive_intersectors.hpp"

using Ray = bvh::Ray<float>;
using PrimitiveIntersector = bvh::ClosestPrimitiveIntersector<Bvh, Triangle>;
using Traverser = bvh::SingleRayTraverser<Bvh>;
using MPTraverser = bvh::SingleRayTraverser<Bvh, 64, bvh::MPNodeIntersector<Bvh, 7, 5>>;

int main() {
    std::vector<Triangle> triangles = parse_ply("model.ply");
    Bvh bvh = build_bvh(triangles);
    std::cout << "BVH has " << bvh.node_count << " nodes" << std::endl;

    PrimitiveIntersector primitive_intersector(bvh, triangles.data());
    Traverser traverser(bvh);
    MPTraverser mp_traverser(bvh);

    std::ofstream traversal_steps_file("traversal_steps.bin", std::ios::out | std::ios::binary);
    std::ofstream intersections_file("intersections.bin", std::ios::out | std::ios::binary);
    std::ofstream ref_traversal_steps_file("ref_traversal_steps.bin", std::ios::out | std::ios::binary);
    std::ofstream ref_intersections_file("ref_intersections.bin", std::ios::out | std::ios::binary);
    std::ofstream correctness_file("correctness.bin", std::ios::out | std::ios::binary);
    std::ofstream different_file("different.bin", std::ios::out | std::ios::binary);
    std::ifstream ray_queries_file("ray_queries.bin", std::ios::in | std::ios::binary);
    float r[7];
    while (ray_queries_file.read(reinterpret_cast<char*>(&r), 7 * sizeof(float))) {
        if (rand() % 100 != 0) continue;

        Ray ray(
                Vector3(r[0], r[1], r[2]),
                Vector3(r[3], r[4], r[5]),
                0.f,
                r[6]
        );

        Traverser::Statistics ref_statistics;
        std::bitset<64> ref_closest_hit_path;
        size_t ref_closest_hit_depth = 0;
        auto ref_result = traverser.traverse(ray, primitive_intersector, ref_statistics,
                                             0, std::bitset<64>(),
                                             ref_closest_hit_path, ref_closest_hit_depth);
        // check whether before_closest_hit_path is correct
        auto first_path = ref_closest_hit_path;
        auto* curr = &bvh.nodes[0];
        while (!curr->is_leaf()) {
            if (first_path.test(0)) curr = &bvh.nodes[curr->first_child_or_primitive + 1];
            else curr = &bvh.nodes[curr->first_child_or_primitive];
            first_path = first_path >> 1;
        }
        if (ref_result) {
            bool found = false;
            for (size_t i = curr->first_child_or_primitive;
                 i < curr->first_child_or_primitive + curr->primitive_count; i++)
                found |= bvh.primitive_indices[i] == ref_result->primitive_index;
            assert(found);
        }

        MPTraverser::Statistics statistics;
        std::bitset<64> closest_hit_path;
        size_t closest_hit_depth = 0;
        auto result = mp_traverser.traverse(ray, primitive_intersector, statistics,
                                            ref_result.has_value() ? 64 : 0, ref_closest_hit_path,
                                            closest_hit_path, closest_hit_depth);

        traversal_steps_file.write(reinterpret_cast<const char*>(&statistics.node_traversed), sizeof(size_t));
        intersections_file.write(reinterpret_cast<const char*>(&statistics.trig_intersections), sizeof(size_t));
        ref_traversal_steps_file.write(reinterpret_cast<const char*>(&ref_statistics.node_traversed), sizeof(size_t));
        ref_intersections_file.write(reinterpret_cast<const char*>(&ref_statistics.trig_intersections), sizeof(size_t));

        bool correctness = false;
        if (ref_result.has_value()) {
            if (result.has_value() && result->intersection.t <= ref_result->intersection.t)
                correctness = true;
        } else
            correctness = true;
        correctness_file.write(reinterpret_cast<const char*>(&correctness), sizeof(bool));

        bool different = (result.has_value() != ref_result.has_value());
        if (result.has_value() && ref_result.has_value()) {
            if (result->intersection.t != ref_result->intersection.t) different = true;
            if (result->intersection.u != ref_result->intersection.u) different = true;
            if (result->intersection.v != ref_result->intersection.v) different = true;
            if (result->primitive_index != ref_result->primitive_index) different = true;
        }
        different_file.write(reinterpret_cast<const char*>(&different), sizeof(bool));

        if (!correctness) {
            std::cerr << r[0] << std::endl;
            std::cerr << r[1] << std::endl;
            std::cerr << r[2] << std::endl;
            std::cerr << r[3] << std::endl;
            std::cerr << r[4] << std::endl;
            std::cerr << r[5] << std::endl;
            std::cerr << r[6] << std::endl;
            std::cerr << statistics.node_traversed << std::endl;
            std::cerr << statistics.trig_intersections << std::endl;
            std::cerr << ref_statistics.node_traversed << std::endl;
            std::cerr << ref_statistics.trig_intersections << std::endl;
            std::cerr << result->intersection.t << std::endl;
            std::cerr << ref_result->intersection.t << std::endl;
            std::cerr << std::endl;
        }
    }
}
