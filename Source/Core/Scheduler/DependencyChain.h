// <copyright file="DependencyChain.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The job group dependency chain
	/// </summary>
	class DependencyChain
	{
	public:
		/// <summary>
		/// Initialize a new instance of the DependencyChain class
		/// </summary>
		DependencyChain();

		/// <summary>
		/// Defines a dependency
		/// </summary>
		void DependsOn(uint32_t parent, uint32_t child);

		/// <summary>
		/// Get dependencies
		/// </summary>
		const std::vector<std::pair<uint32_t, uint32_t>>& GetDependecies() const;

		/// <summary>
		/// Sort the graph
		/// </summary>
		bool SortGraph();

	private:
		/// <summary>
		/// The collection of dependecies
		/// </summary>
		std::vector<std::pair<uint32_t, uint32_t>> m_dependencies;

		/// <summary>
		/// The topologically sorted groups
		/// </summary>
		std::vector<uint32_t> m_sortedGroups;
	};

} // PAL
