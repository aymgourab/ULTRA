#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>

using namespace std;

struct Route {
  string id;
  vector<string> stops;
};

struct Journey {
  vector<string> routes;
  vector<string> transfer_stops;
};

vector<Route> ReadGTFSData(string filename) {
  ifstream file(filename);
  if (!file.is_open()) {
    cout << "Error opening GTFS file" << endl;
    exit(1);
  }

  string line;
  getline(file, line);

  vector<Route> routes;
  while (getline(file, line)) {
    Route route;
    stringstream ss(line);
    string field;

    getline(ss, field, ',');
    route.id = field;

    while (getline(ss, field, ',')) {
      route.stops.push_back(field);
    }

    routes.push_back(route);
  }

  file.close();

  return routes;
}

vector<Journey> FindJourneys(vector<Route> routes, string source, string destination, int max_transfers) {
  vector<Journey> journeys;
  unordered_map<string, vector<Route>> stop_to_routes;

  for (Route route : routes) {
    for (string stop : route.stops) {
      stop_to_routes[stop].push_back(route);
    }
  }

  queue<Journey> queue;
  Journey start_journey;
  start_journey.transfer_stops.push_back(source);
  queue.push(start_journey);

  while (!queue.empty()) {
    Journey journey = queue.front();
    queue.pop();

    string stop = journey.transfer_stops.back();

    if (stop == destination && journey.routes.size() - 1 <= max_transfers) {
      journeys.push_back(journey);
    } else if (journey.routes.size() - 1 < max_transfers) {
      for (Route route : stop_to_routes[stop]) {
        if (journey.routes.empty() || journey.routes.back() != route.id) {
          Journey next_journey = journey;
          next_journey.routes.push_back(route.id);
          next_journey.transfer_stops.push_back(route.stops.back());
          queue.push(next_journey);
        }
      }
    }
  }

  return journeys;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <GTFS file> <source stop ID> <destination stop ID>" << endl;
    return 1;
  }

  string gtfs_file = argv[1];
  string source_stop_id = argv[2];
  string destination_stop_id = argv[3];

  vector<Route> routes = ReadGTFSData(gtfs_file);

  vector<Journey> direct_journeys = FindJourneys(routes, source_stop_id, destination_stop_id, 0);
  vector<Journey> one_transfer_journeys = FindJourneys(routes, source_stop_id, destination_stop_id, 1);
  vector<Journey> two_transfer_journeys = FindJourneys(routes, source_stop_id, destination_stop_id, 2);

  cout << "Direct journeys:";
  for (Journey journey : direct_journeys) {
    cout << " " << journey.routes[0] << "(" << journey.transfer_stops[0] << " > " << journey.transfer_stops[1] << ")";
  }
  cout << endl;

  cout << "Journeys with one transfer:";
  for (Journey journey : one_transfer_journeys) {
    cout << " " << journey.routes[0] << "(" << journey.transfer_stops[0] << " > " << journey.transfer_stops[1] << ") - " << journey.routes[1] << "(" << journey.transfer_stops[1] << " > " << journey.transfer_stops[2] << ")";
  }
  cout << endl;

  cout << "Journeys with two transfers:";
  for (Journey journey : two_transfer_journeys) {
    cout << " " << journey.routes[0] << "(" << journey.transfer_stops[0] << " > " << journey.transfer_stops[1] << ") - " << journey.routes[1] << "(" << journey.transfer_stops[1] << " > " << journey.transfer_stops[2] << ") - " << journey.routes[2] << "(" << journey.transfer_stops[2] << " > " << journey.transfer_stops[3] << ")";
  }
  cout << endl;

  return 0;
}
