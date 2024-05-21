// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include <vector>
#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <string>
#include <deque>



struct Dictionary {
  std::string word;
  int prev = -1;
  bool discovered = false;
};

class Letterman {
 public:
  bool begin_in_dict = false;
  bool end_in_dict = false;

  void getopt(int argc, char** argv) {
    opterr = false;
    int choice;
    int index = 0;
    int sq_count = 0;

    option long_options[] = {
      {"stack", no_argument, nullptr, 's'},
      {"queue", no_argument, nullptr, 'q'},
      {"change", no_argument, nullptr, 'c'},
      {"length", no_argument, nullptr, 'l'},
      {"swap", no_argument, nullptr, 'p'},
      {"help", no_argument, nullptr, 'h'},
      {"begin", required_argument, nullptr, 'b'},
      {"end", required_argument, nullptr, 'e'},
      {"output", required_argument, nullptr, 'o'},
      { nullptr, 0, nullptr, '\0' }
    };

    while ((choice = getopt_long(argc, argv, "sqclphb:e:o:", long_options, &index)) != -1) {
      switch (choice) {
        case 's':
          ++sq_count;
          stack_mode = true;
          break;
        
        case 'q':
          ++sq_count;
          queue_mode = true;
          break;

        case 'c':
          change_mode = true;
          break;
        
        case 'l':
          length_mode = true;
          break;

        case 'p':
          swap_mode = true;
          break;
        
        case 'h':
          std::cout << "Usage: \'./letter\n\t[--stack | -l]\n"
                    <<                    "\t[--queue | -q]\n"
                    <<                    "\t[--change | -c]\n"
                    <<                    "\t[--length | -l]\n"
                    <<                    "\t[--swap | -p]\n"
                    <<                    "\t[--help | -h]\n"
                    <<                    "\t[--begin | -b] <word>\n"
                    <<                    "\t[--end | -e] <word>\n"
                    <<                    "\t[--output | -o] (W|M)\n"
                    <<                    "\t< <TXT Dictionary File>\'\n"
                    << "This program is to read in a dictionary of words, a begin word and"
                    << "an end word, and which type of conversions it is allowed to perform."
                    << "It will try to convert the begin word to the end word following the"
                    << "conversion rules set and use only words in dictionary and make only"
                    << "one change at a time.\n";
          exit(0);
        
        case 'b':
          begin_word = optarg;
          begin_size = begin_word.size();
          begin = true;
          sorted_begin = begin_word;
          sort(sorted_begin.begin(), sorted_begin.end());
          break;
        
        case 'e':
          end_word = optarg;
          end_size = end_word.size();
          end = true;
          sorted_end = end_word;
          sort(sorted_end.begin(), sorted_end.end());
          break;

        case 'o': {
          std::string arg{optarg};
          if (arg == "M") {
            mod_output = true;
          } 
          else if (arg != "W") {
            std::cerr << "Invalid output mode specified\n";
            exit(1);
          }
          break;
        }
      }
    }
    if (sq_count != 1) {
      std::cerr << "Must specify one of stack or queue\n";
      exit(1);
    }
    if (not (change_mode or length_mode or swap_mode)) {
      std::cerr << "Must specify at least one modification mode (change length swap)\n";
      exit(1);
    }
    if (begin_size != end_size and not length_mode) {
      std::cerr << "The first and last words must have the same length when length mode is off\n";
      exit(1);
    }
    if (not begin) {
      std::cerr << "Beginning word not specified\n";
      exit(1);
    }
    if (not end) {
      std::cerr << "Ending word not specified\n";
      exit(1);
    }
  }

  void read_data() {
    char type = '\0';
    std::string temp1;
    std::cin >> type;
    std::cin >> temp1;
    dict_size = std::stoi(temp1);
    Dictionary temp2;

    if (type == 'S') {
      dict.reserve(dict_size);
      std::cin.ignore();

      for (std::size_t i = 0; i < dict_size; ++i) {
        std::cin >> temp1;
        if (temp1.substr(0, 2) == "//") {
          std::getline(std::cin, temp1);
          --i;
        }
        else if (temp1.size() == 0) {
          --i;
        }
        else {
          if (check_word(temp1)) {
            be_in_dict_check(temp1);
            temp2.word = temp1;
            dict.push_back(temp2);
          }
        }
      }
      dict_size = dict.size();
    }
    else {
      dict.reserve(dict_size * 3 / 2);
      std::cin.ignore();

      const std::string chars = "&[!?";
      std::string temp3;
      for (std::size_t j = 0; j < dict_size; ++j) {
        std::cin >> temp1;
        if (temp1.substr(0, 2) == "//") {
          std::getline(std::cin, temp1);
          --j;
        }
        else {
          std::size_t index = temp1.find_first_of(chars);
          if (index != std::string::npos) {
            if (temp1[index] == '&') {
              temp3 = temp1.substr(0, index);
              if (check_word(temp3)) {
                be_in_dict_check(temp3);
                temp2.word = temp3;
                dict.push_back(temp2);
              }

              reverse_string(temp3);
              if (check_word(temp3)) {
                be_in_dict_check(temp3);
                temp2.word = temp3;
                dict.push_back(temp2);
              }
            }
            else if (temp1[index] == '[') {
              std::size_t r_index = temp1.find(']');

              for (std::size_t i = index + 1; i < r_index; ++i) {
                temp3 = temp1.substr(0, index) + temp1[i] + temp1.substr(r_index + 1, temp1.size() - r_index - 1);
                if (check_word(temp3)) {
                  be_in_dict_check(temp3);
                  temp2.word = temp3;
                  dict.push_back(temp2);
                }
              }
            }
            else if (temp1[index] == '!') {
              if (index > 1) {
                temp3 = temp1.substr(0,index) + temp1.substr(index + 1, temp1.size() - index - 1);
                if (check_word(temp3)) {
                  be_in_dict_check(temp3);
                  temp2.word = temp3;
                  dict.push_back(temp2);
                }

                temp3[index - 2] = temp1[index - 1];
                temp3[index - 1] = temp1[index - 2];
                if (check_word(temp3)) {
                  be_in_dict_check(temp3);
                  temp2.word = temp3;
                  dict.push_back(temp2);
                }
              }
            }
            else {
              temp3 = temp1.substr(0, index) + temp1.substr(index + 1, temp1.size() - index - 1);
              if (check_word(temp3)) {
                be_in_dict_check(temp3);
                temp2.word = temp3;
                dict.push_back(temp2);
              }
              
              temp1[index] = temp1[index - 1];
              if (check_word(temp1)) {
                be_in_dict_check(temp1);
                temp2.word = temp1;
                dict.push_back(temp2);
              }
            }
          }
          else {
            if (check_word(temp1)) {
              be_in_dict_check(temp1);
              temp2.word = temp1;
              dict.push_back(temp2);
            }
          }
        }
      }
      dict_size = dict.size();
    }
  }

  void search() {
    search_container.push_back(begin_index);
    ++words_discovered;
    dict[begin_index].discovered = true;
    current_index = begin_index;
    if (current_index == end_index) {
      dict[current_index].prev = current_index;
      return;
    }

    do {
      if (not search_container.empty()) {
        current_index = deque_pop(stack_mode);
      }
      for (std::size_t i = 0; i < dict_size; ++i) {
        if (not dict[i].discovered) {
          if (compare(dict[current_index].word, dict[i].word)) {
            search_container.push_back(static_cast<int>(i));
            dict[i].discovered = true;
            ++words_discovered;
            dict[i].prev = current_index;
            if (static_cast<int>(i) == end_index) {
              return;
            }
          }
        }
      }
    } while (not search_container.empty());
    return;
  }

  void output() {
    if (dict[end_index].prev == -1) {
      std::cout << "No solution, " << words_discovered << " words discovered.\n";
      return;
    }
    else if (dict[end_index].prev == end_index) {
      std::cout << "Words in morph: 1\n" << dict[begin_index].word << '\n';
      return;
    }

    std::vector<int> path;
    for (int i = end_index; i != begin_index; i = dict[i].prev) {
      path.push_back(i);
    }
    std::reverse(path.begin(), path.end());

    std::cout << "Words in morph: " << path.size() + 1 << '\n' << dict[begin_index].word << '\n';
    for (std::size_t i = 0; i < path.size(); ++i) {
      if(mod_output) {
        if (i != 0) {
          std::cout << mod_path(dict[path[i]].word, dict[path[i - 1]].word);
        }
        else {
          std::cout << mod_path(dict[path[i]].word, dict[begin_index].word);
        }
      }
      else {
        std::cout << dict[path[i]].word << '\n';
      }
    }
  }

 private:
  bool stack_mode = false;
  bool queue_mode = false;
  bool change_mode = false;
  bool length_mode = false;
  bool swap_mode = false;
  bool mod_output = false;
  std::string begin_word;
  std::string sorted_begin;
  std::size_t begin_size;
  bool begin = false;
  std::string end_word;
  std::string sorted_end;
  std::size_t end_size;
  bool end = false;
  int begin_index = -1;
  int end_index = -1;
  int current_index = -1;
  std::vector<Dictionary> dict;
  std::size_t dict_size;
  std::deque<int> search_container;
  int words_discovered = 0;

  int deque_pop (const bool &stack_mode) {
    int poped = -1;
    if (stack_mode) {
      poped = search_container.back();
      search_container.pop_back();
    }
    else {
      poped = search_container.front();
      search_container.pop_front();
    }
    return poped;
  }

  void reverse_string( std::string &in) {
    std::string reversed;
    std::size_t size = in.size();
    if (not in.empty()) {
      for (std::size_t i = 0; i < size; ++i) {
        reversed += in[size -1 - i];
      }
    }
    in = reversed;
  }

  void be_in_dict_check(const std::string &word) {
    if (begin_word == word) {
      begin_in_dict = true;
      begin_index = static_cast<int>(dict.size());
    }
    if (end_word == word) {
      end_in_dict = true;
      end_index = static_cast<int>(dict.size());
    }
  }

  bool check_letters(std::string check) {
    sort(check.begin(), check.end());
    return sorted_begin == check;
  }

  bool check_word(const std::string &temp) {
    if (not length_mode) {
      if (begin_size != temp.size()) {
        return false;
      }
      else if (not change_mode) {
        if (check_letters(temp)) {
          return true;
        }
        else {
          return false;
        }
      }
      else {
        return true;
      }
    }
    return true;
  }

  std::string mod_path(const std::string &current, const std::string &previous) {
    std::size_t current_size = current.size();
    std::size_t previous_size = previous.size();

    if (current_size == previous_size) {
      for (std::size_t i = 0; i < current_size; ++i) {
        if (current[i] != previous[i]) {
          if (i >= current_size - 1) {
            return "c," + std::to_string(i) + "," + current[i] + '\n';
          }
          else {
            if (current[i + 1] != previous[i + 1]) {
              return "s," + std::to_string(i) + '\n';
            }
            else {
              return "c," + std::to_string(i) + "," + current[i] + '\n';
            }
          }
        }
      }
    }
    else if (current_size > previous_size) {
      for (std::size_t i = 0; i < previous_size; ++i) {
        if (current[i] != previous[i]) {
          return "i," + std::to_string(i) + "," + current[i] + '\n';
        }
      }
      return "i," + std::to_string(current_size - 1) + "," + current[current_size - 1] + '\n';
    }
    else {
      for (std::size_t i = 0; i < current_size; ++i) {
        if (current[i] != previous[i]) {
          return "d," + std::to_string(i) + '\n';
        }
      }
      return "d," + std::to_string(previous_size - 1) + '\n';
    }

    return std::string();
  }

  bool compare(const std::string &current, const std::string &check_word) const {
    std::size_t current_size = current.size();
    std::size_t check_size = check_word.size();
    bool modified = false;
    bool change_available = change_mode;
    bool swap_available = swap_mode;
    bool length_available = length_mode;
    int diff = abs(static_cast<int>(current_size) - static_cast<int>(check_size));

    if (diff > 1) {
      return false;
    }
    else if (diff == 0){      
      if (change_mode) {
        if (diff != 0) {
          change_available = false;
        }
        else {
          for (std::size_t i = 0; i < current_size; ++i) {
            if (change_available and current[i] != check_word[i]) {
              if (not modified) {
                modified = true;
              }
              else {
                change_available = false;
                modified = false;
              }
            }
            else if ((not change_available) and current[i] != check_word[i]) {
              break;
            }
          }
        }
      }

      if (swap_mode) {
        if (diff != 0) {
          swap_available = false;
        }
        else {
          for (std::size_t i = 0; i < current_size; ++i) {
            if (swap_available and current[i] != check_word[i]) {
              if (i < current_size - 1) {
                if (not modified) {
                  if (current[i] == check_word[i + 1] and current[i + 1] == check_word[i]) {
                    modified = true;
                    ++i;
                  }
                  else {
                    swap_available = false;
                    modified = false;
                  }
                }
                else {
                  swap_available = false;
                  modified = false;
                }
              }
              else { 
                swap_available = false;
                modified = false;
              }
            }
            else if ((not swap_available) and current[i] != check_word[i]) {
              break;
            }
          }
        }
      }
      return swap_available or change_available;
    }
    else {
      if (length_mode) {
        if (diff != 1) {
          length_available = false;
        }
        else {
          if (check_size > current_size) {
            int j = 0;
            for (std::size_t i = 0; i < current_size; ++i) {
              if (current[i] != check_word[j]) {
                if (not modified) {
                  if (current[i] == check_word[j + 1]) {
                    modified = true;
                    ++j;
                  }
                  else {
                    length_available = false;
                  }
                }
                else {
                  length_available = false;
                }
              }
              ++j;
            }
          }
          else {
            int j = 0;
            for (std::size_t i = 0; i < current_size; ++i) {
              if (current[i] != check_word[j]) {
                if (not modified) {
                  if (current[i + 1] == check_word[j]) {
                    modified = true;
                    --j;
                  }
                  else {
                    length_available = false;
                  }
                }
                else {
                  length_available = false;
                }
              }
              ++j;
            }
          }
        }
      }
    return length_available;
    }
  }
};

int main(int argc, char** argv) {
  std::ios_base::sync_with_stdio(false);

  Letterman lm;
  
  lm.getopt(argc, argv);
  lm.read_data();
  if (not lm.begin_in_dict) {
    std::cerr << "Beginning word does not exist in the dictionary\n";
    exit(1);
  }
  if (not lm.end_in_dict) {
    std::cerr << "Ending word does not exist in the dictionary\n";
    exit(1);
  }
  lm.search();
  lm.output();

  return 0;
}