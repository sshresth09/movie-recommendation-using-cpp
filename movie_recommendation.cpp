#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <map>

// Function to load ratings from a CSV file
std::vector<std::vector<int>> loadRatings(const std::string& filename) {
    std::vector<std::vector<int>> ratings;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string value;

        while (std::getline(ss, value, ',')) {
            row.push_back(std::stoi(value));
        }
        ratings.push_back(row);
    }
    return ratings;
}

// Function to calculate cosine similarity between two users
double calculateSimilarity(const std::vector<int>& user1, const std::vector<int>& user2) {
    double dotProduct = 0, normA = 0, normB = 0;

    for (size_t i = 0; i < user1.size(); ++i) {
        dotProduct += user1[i] * user2[i];
        normA += user1[i] * user1[i];
        normB += user2[i] * user2[i];
    }

    if (normA == 0 || normB == 0) return 0; // Avoid division by zero
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

// Predict a user's rating for a specific movie
double predictRating(const std::vector<std::vector<int>>& ratings, int targetUser, int movieIdx) {
    double similaritySum = 0, weightedSum = 0;

    for (size_t otherUser = 0; otherUser < ratings.size(); ++otherUser) {
        if (otherUser == targetUser) continue;
        double similarity = calculateSimilarity(ratings[targetUser], ratings[otherUser]);
        if (ratings[otherUser][movieIdx] > 0) { // Only consider users who rated this movie
            weightedSum += similarity * ratings[otherUser][movieIdx];
            similaritySum += std::abs(similarity);
        }
    }

    if (similaritySum == 0) return 0; // Avoid division by zero
    return weightedSum / similaritySum;
}

// Recommend top N movies for a user
std::vector<std::pair<int, double>> recommendMovies(const std::vector<std::vector<int>>& ratings, int user, int topN) {
    std::vector<std::pair<int, double>> predictions;

    for (size_t movieIdx = 0; movieIdx < ratings[0].size(); ++movieIdx) {
        if (ratings[user][movieIdx] == 0) { // Only predict for unrated movies
            double predictedRating = predictRating(ratings, user, movieIdx);
            predictions.emplace_back(movieIdx, predictedRating);
        }
    }

    // Sort movies by predicted rating in descending order
    std::sort(predictions.begin(), predictions.end(), [](auto& a, auto& b) {
        return b.second > a.second; // Sort in descending order of rating
    });

    // Get the top N recommendations
    if (predictions.size() > static_cast<size_t>(topN)) {
        predictions.resize(topN);
    }

    return predictions;
}

int main() {
    // Load ratings matrix from a CSV file
    std::string filename = "ratings.csv";
    auto ratings = loadRatings(filename);

    // Input: User ID (M) and number of recommendations (N)
    int user, topN;
    std::cout << "Enter the user index (M, 0-indexed): ";
    std::cin >> user;
    std::cout << "Enter the number of recommendations (N): ";
    std::cin >> topN;

    if (user < 0 || user >= ratings.size()) {
        std::cout << "Invalid user index!\n";
        return 1;
    }

    // Get recommendations
    auto recommendations = recommendMovies(ratings, user, topN);

    // Output recommendations
    std::cout << "Top " << topN << " movie recommendations for User " << user + 1 << ":\n";
    for (const auto& recommendation : recommendations) {
        int movieIdx = recommendation.first;
        double predictedRating = recommendation.second;
        std::cout << "Movie " << movieIdx + 1 << " with predicted rating: " << predictedRating << "\n";
    }

    return 0;
}
