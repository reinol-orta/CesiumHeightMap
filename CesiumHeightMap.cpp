#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <array>
#include <cassert>
#include <algorithm>
#include <stdint.h>

using namespace std;

struct Point
{
    int x, y;
};

float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}

class HeightMap
{
private:
    vector<vector<uint8_t>> gridArray;

public:
    const int gridResolution = 512;
    const int totalElements = gridResolution * gridResolution;

    HeightMap(const string &filename)
    {
        ifstream file(filename, ios::binary);

        if (!file)
        {
            cout << "Error No File Found." << endl;
            exit(EXIT_FAILURE);
        }

        vector<uint8_t> bytes;

        // Read byte by byte from the file
        uint8_t byte;
        while (file.read(reinterpret_cast<char *>(&byte), sizeof(uint8_t)))
        {
            bytes.push_back(byte);
        }

        file.close();

        // cout << "Size of bytes vector: " << bytes.size() << " bytes" << std::endl;
        // cout << sizeof(uint8_t) << endl;

        // Print out the array of bytes
        // cout << "Bytes read from the file:" << endl;
        // for (uint8_t b : bytes)
        // {
        //     cout << unsigned(b) << " ";
        // }
        // cout << endl;

        // Create a 2D vector and copy elements from the bytes vector
        vector<vector<uint8_t>> _gridArray(gridResolution, vector<uint8_t>(gridResolution));
        int index = 0;
        for (int i = 0; i < gridResolution; ++i)
        {
            for (int j = 0; j < gridResolution; ++j)
            {
                _gridArray[i][j] = bytes[index++];
            }
        }

        gridArray = _gridArray;
    }

    double calculateSurfaceDistance(const Point &a, const Point &b) const
    {
        const double spacialResolution = 30; // Meters per array index
        const double heightResolution = 11;  // Meters per height value

        // // Calculate distance between two point indices
        // int dx = abs(b.x - a.x);
        // int dy = abs(b.y - a.y);

        // // Calculate the diagonal distance with minimum steps (diagonal movements count as one step)
        // int diagonalSteps = min(dx, dy);
        // int remainingSteps = abs(dx - dy);

        // // Calculate the total distance using diagonal and remaining steps
        // double spacialDistance = (diagonalSteps + remainingSteps) * spacialResolution; // Multiply by spacial Resolution to convert to meters

        double horizonalDistance = sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) * 1.0) * spacialResolution; // Minimum Distance. Doesn't account for height
        cout << "Horizontal Distance Traveled: " << horizonalDistance << endl;

        // Intent to pathfind, to get delta height during path
        int steps = horizonalDistance / spacialResolution;
        int currentPathX = a.x;
        int currentPathY = a.y;
        int currentHeight = gridArray[a.x][b.y];
        int deltaHeight = 0;
        for (int i = 0; i < steps; i++)
        {
            int previousHeight = int(gridArray[currentPathX][currentPathY]);

            if (b.x > currentPathX)
                currentPathX++;
            if (b.y > currentPathY)
                currentPathY++;

            float lerpX = lerp(a.x, b.x, (float(i) / float(steps)));
            float xRoundedUp = ceil(lerpX);
            float xRoundedDown = floor(lerpX);

            float lerpY = lerp(a.y, b.y, (float(i) / float(steps)));
            float yRoundedUp = ceil(lerpY);
            float yRoundedDown = floor(lerpY);
            float pointSquareTL;
            float pointSquareTR;
            float pointSquareBL;
            float pointSqaureBR;

            currentPathX = round(lerpX);
            currentPathY = round(lerpY);

            unsigned int changeInHeight = abs(int(gridArray[currentPathX][currentPathY]) - previousHeight);
            // cout << "Step: " << i + 1 << " change in height: " << changeInHeight << endl;
            deltaHeight += changeInHeight;
        }

        cout << "Height Distance Traveled: " << deltaHeight * heightResolution << endl;

        return horizonalDistance + (deltaHeight * heightResolution);
    }
};

int main()
{
    // Load pre- and post-eruption height maps
    HeightMap preEruptionMap("pre.data");
    HeightMap postEruptionMap("post.data");

    // Specify points A and B in pixel coordinates(x, y);
    // Point pointA(0, 0);
    // Point pointB(100, 1);
    Point start, finish;

    cout << "Enter the coordinates of the starting point (x y): ";
    cin >> start.x >> start.y;

    cout << "Enter the coordinates of the finishing point (x y): ";
    cin >> finish.x >> finish.y;

    cout << "\nStart point: (" << start.x << ", " << start.y << ")\n";
    cout << "Finish point: (" << finish.x << ", " << finish.y << ")\n\n";

    double totalDistancePre = preEruptionMap.calculateSurfaceDistance(start, finish);
    cout << "Surface Distance Pre-Eruption between two points is: " << totalDistancePre << " meters\n\n";

    double totalDistancePost = postEruptionMap.calculateSurfaceDistance(start, finish);
    cout << "Surface Distance Post-Eruption between two points is: " << totalDistancePost << " meters\n\n";

    // Print the difference in surface distance
    cout << "Difference in surface distance: " << totalDistancePost - totalDistancePre << " meters\n";

    return 0;
}
