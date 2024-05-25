# Connect4 game using MCTS
Implementation of Connect 4 game using Monte Carlo Tree Search algorithm

## How to run
- clone the repo
  ```sh
  git clone https://github.com/anirudhnayak27/Connect4_game_using_MCTS.git
  ```
- Change current directory
  ```sh
  cd Connect 4_game_using_MCTS
  ```
- Make build directory
  ```sh
  mkdir build
  ```
- Build Makefile
  ```sh
  make
  ```
- Run the output file which is named `connect_four`
  ```sh
  ./connect_four
  ```
>**Note**
> - Make sure to install make before building Makefile.
> - If you are on vscode, you can install Makefile Tools extension <p align="here"><img src="https://ms-vscode.gallerycdn.vsassets.io/extensions/ms-vscode/makefile-tools/0.10.7/1714536879972/Microsoft.VisualStudio.Services.Icons.Default" height="50px">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </p>
>   Makefile was made in Linux environment, do it is recommended to use vscode for windows to build Makefile.

**Remark** 
> There is a bug in game, the AI(Computer) which is player two that is using MCTS, always goes for a particular column as best move but after the column fills and game in not over, the AI tries to make move in the same column leading to a segmentation fault. Since I have less experience with the Makefilwe and Header files systems I just debugged it by creating a new files and writting the entire game in it. Looking for solutions in origin game(with bug) in Cpp folder.

- To build `Game_After_bug_fixed.cpp`
  ```
  g++ Game_After_bug_fixed.cpp -std=c++17 -o <output_filename>
  ```
  >Note: Can use `g++ Game_After_bug_fixed.cpp -fopenmp -std=c++17 -lstdc++ -o <output_filename>` if above command throws an error.
- To run `Game_After_bug_fixed.cpp`
  ```
  ./<output_filename>
  
## Contribution

Contributions are welcome! If you have any suggestions, improvements, or bug fixes, please submit a pull request or open an issue on the GitHub repository.

## License

This project is licensed under the [MIT License](LICENSE).
