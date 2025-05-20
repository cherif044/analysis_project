#include <vector>
#include <algorithm>
#include <set>
#include <climits>
#include <string>
using namespace std;

struct Student {
    string name;
    int p; // working hours needed
};

struct Lab {
    vector<Student> students;
    int size;
    int current_index;
    int current_hour;
    int actual_working_hours;
};

struct Visit {
    int time;
};
void approximation_algorithm(int L, vector<Lab>& labs, int T, int c, vector<Visit>& visits) {
     
    int inspections = 0;
    visits.clear();

    for (int i=0;i<labs.size();i++) {
        labs[i].current_index = 0;
        labs[i].current_hour = 9;
        labs[i].actual_working_hours = 0;
    }

    while (inspections < c) {
        vector<pair<int, int>> finish_times;
        bool has_students = false;
        for (int i = 0; i < L; i++) {
            if (labs[i].current_index < labs[i].size) {
                int finish_time = labs[i].current_hour + labs[i].students[labs[i].current_index].p;
                pair<int,int>temp2;
                temp2.first=finish_time;
                temp2.second=i;
                finish_times.push_back(temp2);
                has_students = true;
            }
        }

        if (!has_students) break;
        vector<pair<int, double>> inspection_scores;
        for (int i=0;i<finish_times.size();i++) {
            int inspection_time = finish_times[i].first;
            double total_score = 0.0;

            for (int j = 0; j < L; j++) {
    if (labs[j].current_index >= labs[j].size) continue;
    int needed_time = labs[j].students[labs[j].current_index].p;
    int finish_time = labs[j].current_hour + needed_time;
    int diff = finish_time - inspection_time;
    double weighted_diff;
    if (diff < 0) {
        double total_next_p = 0.0;
        int count = 0;

        for (int k = 0; k < L; k++) {
            int next_idx = labs[k].current_index + 1;
            if (next_idx < labs[k].size) {
                total_next_p += labs[k].students[next_idx].p;
                count++;
            }
        }
        double avg_next_p;
        if(count > 0)
        {
            avg_next_p= (total_next_p / count);      
        }
        else{
             avg_next_p=0;
        }
         

        
    } else {
        weighted_diff = diff;
    }

    total_score += needed_time * weighted_diff;
}


            inspection_scores.push_back({ inspection_time, total_score });
        }

        if (inspection_scores.empty()) break;

      int best_time = -1;
        double best_score = INT_MAX;
        for (int i = 0; i < inspection_scores.size(); i++) {
            if (inspection_scores[i].second < best_score) {
                best_score = inspection_scores[i].second;
                best_time = inspection_scores[i].first;
            }
        }

        int inspection_time = best_time;
        if (inspection_time > T + 9) break;

        visits.push_back({ inspection_time });
        inspections++;
        for (int i = 0; i < L; i++) {
            if (labs[i].current_index >= labs[i].size) continue;

            int needed_time = labs[i].students[labs[i].current_index].p;
            int finish_time = labs[i].current_hour + needed_time;

            if (finish_time <= inspection_time) {
                labs[i].actual_working_hours += needed_time;
                labs[i].current_index++;
                labs[i].current_hour = inspection_time;
            }
        }
    }
}


// Utility to generate all combinations of c elements from vec
void generate_combinations(const vector<int>& vec, int c, int start, vector<int>& current, vector<vector<int>>& result) {
    if (current.size() == c) {
        result.push_back(current);
        return;
    }
    for (int i = start; i < vec.size(); ++i) {
        current.push_back(vec[i]);
        generate_combinations(vec, c, i + 1, current, result);
        current.pop_back();
    }
}

// Simulates a schedule and returns total working hours
int simulate_schedule(const vector<int>& inspections, vector<Lab> labs_copy, int T) {
    sort(labs_copy.begin(), labs_copy.end(), [](const Lab& a, const Lab& b) { return a.students[0].name < b.students[0].name; });
    for (auto& lab : labs_copy) {
        lab.current_index = 0;
        lab.current_hour = 9;
        lab.actual_working_hours = 0;
    }

    for (int inspection_time : inspections) {
        for (auto& lab : labs_copy) {
            while (lab.current_index < lab.size) {
                int needed_time = lab.students[lab.current_index].p;
                int finish_time = lab.current_hour + needed_time;
                if (finish_time <= inspection_time && finish_time <= 9 + T) {
                    lab.actual_working_hours += needed_time;
                    lab.current_hour = inspection_time;
                    lab.current_index++;
                }
                else {
                    break;
                }
            }
        }
    }

    // Return total working hours
    int total = 0;
    for (const auto& lab : labs_copy) {
        total += lab.actual_working_hours;
    }
    return total;
}

// Brute-force scheduler
void brute_force_schedule(int L, const vector<Lab>& labs, int T, int c, vector<Visit>& best_visits) {
    set<int> all_finish_times;

    // Gather all possible finish times across all labs
    for (const auto& lab : labs) {
        int time = 9;
        for (const auto& student : lab.students) {
            time += student.p;
            if (time <= 9 + T) {
                all_finish_times.insert(time);
            }
        }
    }

    vector<int> finish_times(all_finish_times.begin(), all_finish_times.end());
    best_visits.clear();

    if (finish_times.empty()) return;  // No possible inspection times at all

    int max_working_hours = INT_MIN;

    // Try combinations of size 1 up to c
    vector<int> current;
    vector<vector<int>> inspection_combinations;

    for (int k = 1; k <= min(c, (int)finish_times.size()); ++k) {
        current.clear();
        inspection_combinations.clear();
        generate_combinations(finish_times, k, 0, current, inspection_combinations);

        for (const auto& inspection_times : inspection_combinations) {
            int total_hours = simulate_schedule(inspection_times, labs, T);
            if (total_hours > max_working_hours) {
                max_working_hours = total_hours;
                best_visits.clear();
                for (int t : inspection_times) {
                    best_visits.push_back(Visit{ t });
                }
            }
        }
    }
}



#include <fstream>
#include <random>
#include <iostream>
#include <iomanip>
int main() {
    std::ofstream fout("lab_scheduling_results.csv");
    if (!fout.is_open()) {
        std::cerr << "Failed to open output file\n";
        return 1;
    }

    // Write CSV header
    fout << "InstanceID,Labs,LabDetails,T,C,"
        << "BruteForceWorkingHours,ApproxWorkingHours,Ratio\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist_labs(1, 7);
    std::uniform_int_distribution<> dist_students(1, 15);
    std::uniform_int_distribution<> dist_hours(1, 5);

    // New distributions for T and c
    std::uniform_int_distribution<> dist_T(5, 15);  // Total working time limit between 5 and 15
    std::uniform_int_distribution<> dist_c(1, 5);   // Number of inspections between 1 and 5

    for (int instance_id = 1; instance_id <= 10000; ++instance_id) {
        int L = dist_labs(gen);
        int T = dist_T(gen);
        int c = dist_c(gen);

        std::vector<Lab> labs(L);

        // Generate labs and students randomly
        for (int i = 0; i < L; ++i) {
            int n = dist_students(gen);
            labs[i].students.clear();
            for (int j = 0; j < n; ++j) {
                Student s;
                s.name = std::string(1, 'A' + j);  // simple alphabetical student names
                s.p = dist_hours(gen);
                labs[i].students.push_back(s);
            }
            labs[i].size = n;
        }

        // Run approximation algorithm
        std::vector<Visit> approx_visits;
        approximation_algorithm(L, labs, T, c, approx_visits);
        int approx_hours = 0;
        for (const auto& lab : labs) approx_hours += lab.actual_working_hours;

        // Run brute force algorithm on a fresh copy of labs
        std::vector<Lab> labs_copy = labs;
        std::vector<Visit> bf_visits;
        brute_force_schedule(L, labs_copy, T, c, bf_visits);

        // Extract inspection times from bf_visits
        std::vector<int> bf_times;
        for (auto& v : bf_visits) bf_times.push_back(v.time);

        int brute_force_hours = simulate_schedule(bf_times, labs, T);

        // Calculate ratio, protect division by zero
        double ratio = (approx_hours == 0) ? 0.0 : double(approx_hours) / brute_force_hours;

        // Serialize lab details as a string: LabCount:[Lab0Name-p,...];[Lab1Name-p,...];...
        std::string lab_details;
        for (int i = 0; i < L; ++i) {
            lab_details += "[";
            for (size_t j = 0; j < labs[i].students.size(); ++j) {
                lab_details += labs[i].students[j].name + "-" + std::to_string(labs[i].students[j].p);
                if (j + 1 < labs[i].students.size()) lab_details += ",";
            }
            lab_details += "]";
            if (i + 1 < L) lab_details += ";";
        }

        // Output CSV line
        fout << instance_id << "," << L << ",\"" << lab_details << "\"," << T << "," << c << ","
            << brute_force_hours << "," << approx_hours << "," << std::fixed << std::setprecision(3) << ratio << "\n";

        if (instance_id % 1000 == 0) {
            std::cout << "Processed " << instance_id << " instances\n";
        }
    }

    fout.close();
    std::cout << "Data generation complete, saved to lab_scheduling_results.csv\n";
    return 0;
}
