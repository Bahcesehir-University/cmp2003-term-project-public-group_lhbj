#include "analyzer.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>


struct TripData {
            std::string PickupZoneID;
            int PickupDateTime;
        };

std::unordered_map<std::string, std::vector<TripData>> csv_data_clean;
std::vector<ZoneCount> zone_count;


void TripAnalyzer::ingestFile(const std::string& csvPath) {

            csv_data_clean.clear();
            zone_count.clear();

            std::ifstream file(csvPath);

            std::string line;
            if (file.is_open()){
                        
                        //static const std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}$)");
                        
                        getline(file, line);
                        while(getline(file, line)){
                            
                                    if (line.empty()) {
                                                continue; 
                                    }
                                                    
                                    std::stringstream ss(line);
                                    std::string token;
                                    std::vector<std::string> temp;
                                    while (getline(ss, token, ',')) {
                                                temp.emplace_back(std::move(token));
                                    }
                                    std::tm date = {};
                                    std::istringstream international(temp[3]);
                                    international >> std::get_time(&date, "%Y-%m-%d %H:%M");
                                    //if (!internaitonal.fail() || !ss.rdbuf()->in_avail() != 0
                                    if (temp.size() < 6 || international.fail() || international.rdbuf()->in_avail() != 0) {
                                                continue;
                                    }
                                    
                                    TripData t;
                                    t.PickupZoneID = temp[1];
                                    t.PickupDateTime = std::stoi(temp[3].substr(11, 2));
                                    
                                    csv_data_clean[temp[1]].push_back(t);
                                    zone_count.push_back({temp[1], static_cast<int>(sv_data_clean[temp[1]].size())});
                                    
                        }
                        
                        file.close();
            }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {


            /*zone_count.reserve(csv_data_clean.size());
            
            for (const auto& [zoneID, trips] : csv_data_clean){
                        zone_count.push_back({zoneID, static_cast<int>(trips.size())});
            }*/
            
            std::sort(zone_count.begin(), zone_count.end(), [](const auto& a, const auto& b) {
                        if (a.count != b.count) {
                                    return a.count > b.count;
                        } else {
                                    return a.zone < b.zone;
                        }
            });
            
            int max = std::min(k, static_cast<int>(zone_count.size()));

            
            return std::vector<ZoneCount> (zone_count.begin(), zone_count.begin() + max);
}



std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {

          
            std::vector<SlotCount> slot_count;
            std::unordered_map<std::string, std::unordered_map<int, long long>> time_count;

            for (const auto& [zoneID, trips] : csv_data_clean) {
                        for (const auto& trip : trips) {
                                    time_count[zoneID][trip.PickupDateTime]++; 
                        }
            }

            for (const auto& [zoneID, times] : time_count){
                        for (const auto& [time, count] : times){
                                    slot_count.push_back({zoneID, time, count});      
                        }
            }


            std::sort(slot_count.begin(), slot_count.end(), [](const auto& a, const auto& b) {
                        if (a.count != b.count) {
                                    return a.count > b.count;
                        }
                        if (a.zone != b.zone) {
                                    return a.zone < b.zone;
                        }
                        return a.hour < b.hour;
            });

            int max = std::min(k, static_cast<int>(slot_count.size()));
                 

            return std::vector<SlotCount> (slot_count.begin(), slot_count.begin() + max);
}
