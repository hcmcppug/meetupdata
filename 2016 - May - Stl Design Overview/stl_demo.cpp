#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <regex>
#include <experimental/string_view>
#include <boost/coroutine/all.hpp>

using namespace std;

struct football_match_result {
  string local_team;
  string visitor_team;
  pair<int, int> score;
  size_t f_round;
};


ostream & operator<<(ostream & out,
                     football_match_result const & fmr) {
  out << setw(15) << left << fmr.local_team
      << right << setw(2) << fmr.score.first
      << '-' << left << setw(2)
      << fmr.score.second
      << right << setw(15) <<
    fmr.visitor_team << right << setw(10) << fmr.f_round;
  return out;
}

using f_match_cref_t = reference_wrapper<football_match_result const>;


class league : public vector<football_match_result> {
public:
  static constexpr size_t k_num_matches_per_round = 10;
  static constexpr size_t k_num_rounds = 38;

  using base = vector<football_match_result>;

  template <class InputIterator>
  league(InputIterator b, InputIterator e) : base(b, e) {}
};


using coro_t = boost::coroutines::coroutine<football_match_result>;


void parse_football_league(istream & stream,
                           coro_t::push_type & yield) {

  static regex const football_result_re
    (R"--(((?:[[:alpha:]]|\s)+),(\d+)-(\d+),((?:[[:alpha:]]|\s)+))--");

  size_t result_number{0u};
  string line;
  do {
    getline(stream, line);

    //remove trailing space
    line = string
      (cbegin(line),
       find_if(crbegin(line), crend(line), [](char c) { return !isspace(c); }).base());
    if (line.empty()) continue;

    smatch m;
    if (regex_match(line, m, football_result_re)) {
      yield(football_match_result{m[1].str(),
            m[4].str(), {stoi(m[2].str()),stoi(m[3].str())}, (result_number++ / 10) + 1});
    }
  } while (stream && result_number < 380);
  assert(result_number == 380);
}


vector<f_match_cref_t>
all_matches_for_team
(league const & f_league,
 string const & team_name) {
  vector<f_match_cref_t> team_results(begin(f_league),
                                      end(f_league));
  team_results.erase
    (
     remove_if(begin(team_results), end(team_results),
               ([&](football_match_result const & f_match) {
                 return f_match.local_team != team_name &&
                   f_match.visitor_team != team_name;
               })),
     end(team_results));

  return team_results;
}


using classification_table_t = vector<pair<string, int>>;


classification_table_t calculate_classification(league const & l) {
  unordered_map<string, int> teams_points;

  for (auto const & f_match : l) {
    if (f_match.score.first > f_match.score.second) {
      teams_points[f_match.visitor_team] += 0;
      teams_points[f_match.local_team] += 3;
    }
    else if (f_match.score.first < f_match.score.second) {
      teams_points[f_match.visitor_team] += 3;
      teams_points[f_match.local_team] += 0;
    }
    else {
      teams_points[f_match.local_team] += 1;
      teams_points[f_match.visitor_team] += 1;
    }
  }

  classification_table_t classification
    (begin(teams_points), end(teams_points));

  sort(begin(classification), end(classification),
       [](auto const & lhs, auto const & rhs) {
         return lhs.second > rhs.second;
       });
  return classification;
}


vector<f_match_cref_t>
won_matches_as_visitor(league const & l,
                       string const & team_name) {

  vector<f_match_cref_t> result(begin(l), end(l));

  auto partition_point =
    std::partition(begin(result),
                   end(result),
                   [&team_name](football_match_result const & m) {
                     return m.visitor_team == team_name &&
                     m.score.second > m.score.first;
                   });

  result.erase(partition_point,
               end(result));
  return result;
}


std::ostream & print_matches_header(std::ostream & out) {
  out << setw(15) << left << "Local team"
      << setw(5) << "Result"
      << right << setw(15) << "Visitor team"
      << setw(10) << "Round" << '\n';
  out << "----------------------------------------------\n";
  return out;
}


int main() {
  ifstream league_stream("liga_futbol2015-2016.txt");
  if (!league_stream) return -1;

  
  coro_t::pull_type league_results
    ([&league_stream](coro_t::push_type & yield) {
      parse_football_league(league_stream, yield);
    });

  league const f_league(begin(league_results), end(league_results));

  print_matches_header(cout);
  for (auto const & r : all_matches_for_team(f_league, "Real Madrid")) {
    cout << r << '\n';
  }
  cout << "\n---------------------------------------------\n";

  for (auto const & entry : calculate_classification(f_league)) {
    cout << setw(15) << left <<
      entry.first << setw(3) << right << entry.second << '\n';
  }
  
  cout << "\n---------------------------------------------\n";
  cout << "Malaga won these matches as visitor:\n\n";
  print_matches_header(cout);
  for (auto const & m : won_matches_as_visitor(f_league, "Malaga")) {
    cout << m << '\n';
  }
}
