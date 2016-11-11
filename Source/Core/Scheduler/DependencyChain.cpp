// <copyright file="DependencyChain.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "DependencyChain.h"

namespace PAL
{
	DependencyChain::DependencyChain() :
		_dependencies(),
		_sortedGroups()
	{
	}

	void DependencyChain::DependsOn(uint32_t parent, uint32_t child)
	{
		_dependencies.emplace_back(parent, child);

		if (!SortGraph())
		{
			throw std::invalid_argument("The chain is no longer valid!");
		}

		std::wcout << L"Sorted: ";
		for (uint32_t n : _sortedGroups)
		{
			std::wcout << n << L' ';
		}

		std::wcout << std::endl;
	}

	const std::vector<std::pair<uint32_t, uint32_t>>& DependencyChain::GetDependecies() const
	{
		return _dependencies;
	}

	bool DependencyChain::SortGraph()
	{
		// Clear old sorted list
		_sortedGroups.clear();
		std::vector<std::pair<uint32_t, uint32_t>> graph(_dependencies);

		// Find all nodes with no incoming edges
		std::set<uint32_t> edgeNodes;
		
		// Find all nodes
		for (const auto& edge : graph)
		{
			edgeNodes.insert(edge.first);
			edgeNodes.insert(edge.second);
		}

		// Remove a node that has incoming edges
		for (const auto& edge : graph)
		{
			edgeNodes.erase(edge.second);
		}

		// Sort the graph
		while (!edgeNodes.empty())
		{
			// Get an edge node
			uint32_t node = *edgeNodes.begin();
			edgeNodes.erase(node);

			// Add the edge node to the sorted list
			_sortedGroups.push_back(node);

			if (!graph.empty())
			{
				// Find the new fringe
				std::set<uint32_t> nodeTargets;
				for (const auto& edge : graph)
				{
					if (edge.first == node)
					{
						nodeTargets.insert(edge.second);
					}
				}

				// Remove out edges
				graph.erase(
					remove_if(
						graph.begin(),
						graph.end(),
						[node](const auto& edge)
				{
					return edge.first == node;
				}));

				// Find new edge nodes
				for (uint32_t targetNode : nodeTargets)
				{
					// Check if there are any incoming edges
					bool hasIncomingEdge = false;
					for (const auto& edge : graph)
					{
						if (edge.second == targetNode)
						{
							hasIncomingEdge = true;
							break;
						}
					}

					// Add to edge nodes if no incoming edges
					if (!hasIncomingEdge)
					{
						edgeNodes.insert(targetNode);
					}
				}
			}
		}

		// Verify that the graph is not acyclic
		bool isAcyclic = true;
		if (!graph.empty())
		{
			// There were nodes that create a circle
			isAcyclic = false;
		}

		return isAcyclic;
	}

} // PAL
