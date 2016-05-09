#include <iostream>
#include <string>
#include <vector>
#include <algorithm>





namespace s = std;

struct food {
  s::string food_name;
  double average_user_score;
};

s::ostream & operator<<(s::ostream & out, food const & f) {
  out << f.food_name << ": " << f.average_user_score;
  return out;
}

struct more_delicious {
  bool operator()(food const & f1, food const & f2) const {
    return f1.average_user_score > f2.average_user_score;
  }
};

int main() {
  s::vector<food const> const foods =
    {{"pho", 8.1},
     {"com tam", 7.6},
     {"bun cha", 9.0},
     {"banh can cua", 7.8},
     {"banh mi", 6.8},
     {"com rang bo", 5.9}};

  using food_cref = s::reference_wrapper<food const>;

  s::vector<food_cref>
    more_to_less_delicious(s::begin(foods), s::end(foods));

  s::sort(s::begin(more_to_less_delicious),
          s::end(more_to_less_delicious),
          more_delicious{});

  s::cout << "Most delicious foods\n\n";
  for (auto const & food : more_to_less_delicious) {
    s::cout << '\t' << food << '\n';
  }

  s::vector<food_cref>
  only_really_delicious(s::begin(foods), s::end(foods));

  struct score_more_than {
    score_more_than(double score) : score_{score} {}
    bool operator()(food const & f) const { return f.average_user_score >= score_; };

    double const score_;
  };

  auto partition_point = s::partition(s::begin(only_really_delicious),
                                      s::end(only_really_delicious),
                                      score_more_than{8.9});

  s::cout << "\n\nReally delicious foods\n\n";
  for (auto it_food = s::begin(only_really_delicious); it_food != partition_point; ++it_food)
    s::cout << '\t' << *it_food << '\n';
}
