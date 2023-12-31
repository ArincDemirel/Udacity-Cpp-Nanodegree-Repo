#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    this->start_node = &m_Model.FindClosestNode(start_x, start_y);
    this->end_node = &m_Model.FindClosestNode(end_x, end_y);
}


// Implement the CalculateHValue method.
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node)
{
    distance =node->distance(*this->end_node);
    return distance;
}

//  Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value.
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node)
{
    current_node->FindNeighbors();
    for (auto neighbor : current_node->neighbors)
    {
        neighbor->parent = current_node;
        neighbor->g_value = current_node->g_value + current_node->distance(*neighbor);
        neighbor->h_value = this->CalculateHValue(neighbor);
        if (neighbor->visited != true)
        {
            this->open_list.push_back(neighbor);
            neighbor->visited = true;
        }
    }
}

// Complete the NextNode method to sort the open list and return the next node.
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.
bool Compare(RouteModel::Node *a, RouteModel::Node *b)
{
    float cost_a = a->g_value + a->h_value;
    float cost_b = b->g_value + b->h_value;
    return cost_a > cost_b;
}

RouteModel::Node *RoutePlanner::NextNode()
{
    std::sort(this->open_list.begin(), this->open_list.end(), Compare);
    auto next_node = this->open_list.back();  // Create a pointer to the node with the lowest sum
    open_list.pop_back(); // Create a pointer to the node with the lowest sum
    return next_node;
}


// Complete the ConstructFinalPath method to return the final path found from your A* search.
// - This method should take the current (final) node as an argument and iteratively follow the
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node)
{
    
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    float current_x = current_node->x;
    float current_y = current_node->y;
    path_found.push_back(*current_node);
    
    // Iterate through the chain of parents until the starting node is found
    while (current_node->parent != nullptr) {
        
        distance += current_node->distance(*current_node->parent);
        
        current_node = current_node->parent;
        current_x = current_node->x;
        current_y = current_node->y;
       path_found.push_back(*current_node) ;//Add the start node to the path
    }
    std::reverse(path_found.begin(), path_found.end());// Reverse the vector to have the correct order
    distance *= m_Model.MetricScale();  // Multiply the distance by the scale of the map to get meters

    return path_found;
}


//   A* Search algorithm
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch()
{
    RouteModel::Node *current_node = nullptr;

    // Initialize the starting node.
    current_node = this->start_node;
    current_node->g_value = 0.0f;
    current_node->h_value = this->CalculateHValue(current_node);
    current_node->visited = true;
    open_list.push_back(current_node);

    while (open_list.size() > 0)
    {
        // Get the next node
        auto next_node = this->NextNode();
        if (next_node->x == this->end_node->x && next_node->y == this->end_node->y)
        {
            m_Model.path = this->ConstructFinalPath(next_node);
            return;
        }
        AddNeighbors(next_node);
    }

    std::cout << "No path found!"
              << "\n";
    return;
}
