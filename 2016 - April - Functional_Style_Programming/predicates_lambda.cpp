#include <iostream>
#include <string>
#include <vector>
#include <algorithm>





namespace s = std;

struct food {
  std::string food_name;
  double average_user_score;
};

std::ostream & operator<<(std::ostream & out, food const & f) {
  out << f.food_name << ": " << f.average_user_score;
  return out;
}

int main() {
  std::vector<food const> const foods =
    {{"pho", 8.1},
     {"com tam", 7.6},
     {"bun cha", 9.0},
     {"banh canh cua", 7.8},
     {"banh mi", 6.8},
     {"com rang bo", 5.9}};

  using food_cref = std::reference_wrapper<food const>;

  std::vector<food_cref>
    more_to_less_delicious(s::begin(foods), s::end(foods));

  s::sort(s::begin(more_to_less_delicious),
          s::end(more_to_less_delicious),
          [](food const & f1,
          food const & f2) { return f1.average_user_score > f2.average_user_score; });

  std::cout << "Most delicious foods\n\n";
  for (auto const & food : more_to_less_delicious) {
    std::cout << '\t' << food << '\n';
  }

  std::vector<food_cref>
  only_really_delicious(s::begin(foods), s::end(foods));

  auto partition_point =
    s::partition(s::begin(only_really_delicious),
                 s::end(only_really_delicious),
                 [score = 8.9](food const & f)
                 { return f.average_user_score > score; });

  s::cout << "\n\nReally delicious foods\n\n";
  for (auto it_food = s::begin(only_really_delicious); it_food != partition_point; ++it_food)
    std::cout << '\t' << *it_food << '\n';
}
