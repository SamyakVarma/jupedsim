// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include "AABB.hpp"
#include "Point.hpp"
#include "GenericAgent.hpp"

#include <vector>
#include <algorithm>
#include <cmath>

class HeatMap {
public:
    HeatMap(const AABB& bounds, double resolution = 0.5, double initialTemp = 20.0)
        : _bounds(bounds), _resolution(resolution), _ambientTemp(initialTemp) {
        _width = static_cast<size_t>(std::ceil((bounds.xmax - bounds.xmin) / resolution)) + 1;
        _height = static_cast<size_t>(std::ceil((bounds.ymax - bounds.ymin) / resolution)) + 1;
        _grid.resize(_width * _height, initialTemp);
        _smokeGrid.resize(_width * _height, 0.0);
        _isWalkable.resize(_width * _height, true);
    }

    void SetWalkable(size_t x, size_t y, bool walkable) {
        if (x < _width && y < _height) {
            _isWalkable[y * _width + x] = walkable;
            if (!walkable) _grid[y * _width + x] = _ambientTemp;
        }
    }

    void Update(double dT, const std::vector<GenericAgent>& agents, double ambientTemp) {
        _ambientTemp = ambientTemp;
        // 1. Heat emission from agents
        for (const auto& agent : agents) {
            AddHeat(agent.pos, 30.0 * dT); // Each agent emits 30 units of heat per second
        }

        // 2. Heat dissipation (diffusion and equilibration with ambient)
        std::vector<double> nextGrid = _grid;
        std::vector<double> nextSmokeGrid = _smokeGrid;
        double diffusionRate = 0.05;      // Realistic diffusion
        double ambientExchangeRate = 0.1; // Exchange with air
        double smokeDiffusionRate = 0.8;
        double smokeDissipationRate = 0.02;

        for (size_t y = 0; y < _height; ++y) {
            for (size_t x = 0; x < _width; ++x) {
                size_t idx = y * _width + x;
                if (!_isWalkable[idx]) continue;

                double currentTemp = _grid[idx];
                double currentSmoke = _smokeGrid[idx];
                
                // Diffusion logic (respecting walkable boundaries)
                double sumNeighbors = 0.0;
                double sumSmokeNeighbors = 0.0;
                int count = 0;
                auto checkNeighbor = [&](int nx, int ny) {
                    if (nx >= 0 && nx < (int)_width && ny >= 0 && ny < (int)_height) {
                        size_t nIdx = ny * _width + nx;
                        if (_isWalkable[nIdx]) {
                            sumNeighbors += _grid[nIdx];
                            sumSmokeNeighbors += _smokeGrid[nIdx];
                            count++;
                        }
                    }
                };

                checkNeighbor(x - 1, y);
                checkNeighbor(x + 1, y);
                checkNeighbor(x, y - 1);
                checkNeighbor(x, y + 1);
                
                if (count > 0) {
                    double averageNeighbor = sumNeighbors / count;
                    nextGrid[idx] += (averageNeighbor - currentTemp) * diffusionRate * dT * 20.0;
                    
                    double averageSmokeNeighbor = sumSmokeNeighbors / count;
                    nextSmokeGrid[idx] += (averageSmokeNeighbor - currentSmoke) * smokeDiffusionRate * dT * 20.0;
                }

                // Exchange with ambient temperature
                nextGrid[idx] += (_ambientTemp - nextGrid[idx]) * ambientExchangeRate * dT;
                
                // Smoke dissipation
                nextSmokeGrid[idx] -= nextSmokeGrid[idx] * smokeDissipationRate * dT;
                if (nextSmokeGrid[idx] < 0.0) nextSmokeGrid[idx] = 0.0;
                
                // Cap temperature at a realistic maximum (e.g., 50C)
                if (nextGrid[idx] > 50.0) nextGrid[idx] = 50.0;
            }
        }
        _grid = std::move(nextGrid);
        _smokeGrid = std::move(nextSmokeGrid);
    }

    double GetTemperature(Point pos) const {
        if (!_bounds.Inside(pos)) return 0.0;
        size_t x = static_cast<size_t>((pos.x - _bounds.xmin) / _resolution);
        size_t y = static_cast<size_t>((pos.y - _bounds.ymin) / _resolution);
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        return _grid[y * _width + x];
    }

    double GetSmoke(Point pos) const {
        if (!_bounds.Inside(pos)) return 0.0;
        size_t x = static_cast<size_t>((pos.x - _bounds.xmin) / _resolution);
        size_t y = static_cast<size_t>((pos.y - _bounds.ymin) / _resolution);
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        return _smokeGrid[y * _width + x];
    }

    const std::vector<double>& Grid() const { return _grid; }
    const std::vector<double>& SmokeGrid() const { return _smokeGrid; }
    size_t Width() const { return _width; }
    size_t Height() const { return _height; }
    const AABB& Bounds() const { return _bounds; }
    double Resolution() const { return _resolution; }
    double AmbientTemperature() const { return _ambientTemp; }

private:
    void AddHeat(Point pos, double amount) {
        if (!_bounds.Inside(pos)) return;
        size_t x = static_cast<size_t>((pos.x - _bounds.xmin) / _resolution);
        size_t y = static_cast<size_t>((pos.y - _bounds.ymin) / _resolution);
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        _grid[y * _width + x] += amount;
    }

    public:
    void AddSmoke(Point pos, double amount) {
        if (!_bounds.Inside(pos)) return;
        size_t x = static_cast<size_t>((pos.x - _bounds.xmin) / _resolution);
        size_t y = static_cast<size_t>((pos.y - _bounds.ymin) / _resolution);
        x = std::min(x, _width - 1);
        y = std::min(y, _height - 1);
        _smokeGrid[y * _width + x] += amount;
    }
    private:

    AABB _bounds;
    double _resolution;
    size_t _width;
    size_t _height;
    std::vector<double> _grid;
    std::vector<double> _smokeGrid;
    std::vector<bool> _isWalkable;
    double _ambientTemp{20.0};
};
