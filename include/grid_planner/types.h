
#ifndef NAEX_TYPES_H
#define NAEX_TYPES_H

#include <Eigen/Dense>
#include <memory>
#include <mutex>

// #define K_NEIGHBORS 32

namespace naex {
typedef std::mutex Mutex;
typedef std::recursive_mutex RMutex;
typedef std::lock_guard<Mutex> Lock;
typedef std::lock_guard<RMutex> RLock;

// Basic floating-point and index types
typedef float Elem;
typedef Elem Value;
// typedef uint32_t Index;
typedef int Index;
// typedef size_t Index;

// Arrays and matrices
typedef Eigen::Matrix<Value, 2, 1, Eigen::DontAlign> Vec2;
typedef Eigen::Map<Vec2> Vec2Map;
typedef Eigen::Map<const Vec2> ConstVec2Map;
static_assert(sizeof(Vec2) == 2 * sizeof(Value));

typedef Eigen::Matrix<Value, 3, 1, Eigen::DontAlign> Vec3;
typedef Eigen::Map<Vec3> Vec3Map;
typedef Eigen::Map<const Vec3> ConstVec3Map;
static_assert(sizeof(Vec3) == 3 * sizeof(Value));

typedef Eigen::Matrix<Value, 4, 1, Eigen::DontAlign> Vec4;
typedef Eigen::Map<Vec4> Vec4Map;
typedef Eigen::Map<const Vec4> ConstVec4Map;
static_assert(sizeof(Vec4) == 4 * sizeof(Value));

typedef Eigen::Matrix<Value, 3, 3, Eigen::DontAlign> Mat3;
typedef Eigen::Matrix<Value, 4, 4, Eigen::DontAlign> Mat4;
typedef Eigen::Matrix<Value, 3, Eigen::Dynamic, Eigen::DontAlign> Mat3X;
typedef Eigen::Map<Mat3X> MatXMap;
typedef Eigen::Map<const Mat3X> ConstMatXMap;

typedef Eigen::Quaternion<Value, Eigen::DontAlign> Quat;
typedef Eigen::Map<Quat> QuatMap;
typedef Eigen::Map<const Quat> ConstQuatMap;

typedef std::vector<Index> Indices;

// Vertex and edge indices
// TODO: Rename both to Index (to be used elsewhere too).
typedef Index Vertex;
typedef Index Edge;
// Edge cost or length
typedef Elem Cost;

enum Flags {
  // Point was updated including its neighborhood. Otherwise it's queued for
  // updated.
  UPDATED = 1 << 0,
  // A static point, not dynamic or empty, necessary for being traversable.
  STATIC = 1 << 1,
  // Approximately horizontal orientation based on normal direction,
  // necessary condition for being traversable.
  HORIZONTAL = 1 << 2,
  // Near another actor.
  ACTOR = 1 << 3,
  // A point at the edge, i.e. a frontier.
  EDGE = 1 << 4,
  // Traversable based on terrain roughness and obstacles in neighborhood.
  TRAVERSABLE = 1 << 5
};

const Index INVALID_INDEX = std::numeric_limits<Index>::max();
const Vertex INVALID_VERTEX = std::numeric_limits<Vertex>::max();

class Point {
public:
  Point() {}

  Value position_[3] = {std::numeric_limits<Value>::quiet_NaN(),
                        std::numeric_limits<Value>::quiet_NaN(),
                        std::numeric_limits<Value>::quiet_NaN()};
  // Geometric features
  // TODO: More compact normal representation? Maybe just for sharing,
  // impacts on memory is small compared to neighbors.
  // TODO: Switch to compact (u)int8 types where possible.
  Value normal_[3] = {std::numeric_limits<Value>::quiet_NaN(),
                      std::numeric_limits<Value>::quiet_NaN(),
                      std::numeric_limits<Value>::quiet_NaN()};
  //    int8 normal_[3];
  // Normal scale is common to all points.
  // Number of points used in normal computation.
  uint8_t normal_support_{0};
  // Roughness features (in neighborhood radius).
  // from ball neighborhood
  Value ground_diff_std_{std::numeric_limits<Value>::quiet_NaN()};
  // circle in ground plane
  Value min_ground_diff_{std::numeric_limits<Value>::quiet_NaN()};
  Value max_ground_diff_{std::numeric_limits<Value>::quiet_NaN()};
  Value mean_abs_ground_diff_{std::numeric_limits<Value>::quiet_NaN()};
  // Viewpoint (for occupancy assessment and measurement distance)
  Value viewpoint_[3];
  // Distance (Euclidean + time) to this actor and other actors.
  Value dist_to_actor_{std::numeric_limits<Value>::quiet_NaN()};
  Value actor_last_visit_{std::numeric_limits<Value>::quiet_NaN()};
  Value dist_to_other_actors_{std::numeric_limits<Value>::quiet_NaN()};
  Value other_actors_last_visit_{std::numeric_limits<Value>::quiet_NaN()};
  //    Value prob_visible_{0};
  Value coverage_{0};
  Value self_coverage_{0};
  // Distance to nearest obstacle (non horizontal point).
  Value dist_to_obstacle_{std::numeric_limits<Value>::quiet_NaN()};
  // Point flags accoring to Flags.
  uint8_t flags_{0};
  // Number of occurences of empty/occupied state.
  uint8_t num_empty_{0};
  uint8_t num_occupied_{0};
  Value dist_to_plane_{std::numeric_limits<Value>::quiet_NaN()};
  // Number of obstacle points in clearance cylinder.
  uint8_t num_obstacle_pts_{0};
  // Number of obstacles nearby.
  uint8_t num_obstacle_neighbors_{0};
  // Edge flag.
  //    uint8_t edge_;
  // Number of edge points nearby.
  uint8_t num_edge_neighbors_{0};
  // Label based on normal direction (normal already needs graph).
  //    uint8_t normal_label_;
  // Label based on terrain roughness.
  //    uint8_t functional_label_;
  // Planning costs and rewards
  Value path_cost_{std::numeric_limits<Value>::quiet_NaN()};
  Value reward_{std::numeric_limits<Value>::quiet_NaN()};
  Value relative_cost_{std::numeric_limits<Value>::quiet_NaN()};
};

class Neighborhood {
public:
  Neighborhood() {}

  // TODO: Make K_NEIGHBORS a parameter.
  static const Index K_NEIGHBORS = 48;
  Value position_[3] = {std::numeric_limits<Value>::quiet_NaN(),
                        std::numeric_limits<Value>::quiet_NaN(),
                        std::numeric_limits<Value>::quiet_NaN()};
  // NN Graph
  // Number of valid entries in neighbors_ and distances_.
  Index neighbor_count_{0};
  Index neighbors_[K_NEIGHBORS] = {0};
  // Treat zero distance and cost as invalid.
  Value distances_[K_NEIGHBORS] = {0};
  Value costs_[K_NEIGHBORS] = {0};
  // Index state from IndexState enum.
  // uint8_t index_state_;
};

} // namespace naex

#endif // NAEX_TYPES_H
