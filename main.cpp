#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <regex>

class Display
{
public:
	static void Clear()
	{
		GotoXY(0, 9);
		std::cout << "                                           ";
	}

	static void GotoXY(int x, int y)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (SHORT)x, (SHORT)y });
	}

	static void Out_Result(int x, int y, char mark)
	{
		if (x > 2 || x < 0)
			return;
		x == 0 ? x = 6 : x == 1 ? x = 8 : x == 2 ? x = 10 : x;
		y == 0 ? y = 2 : y == 1 ? y = 4 : y == 2 ? y = 6 : y;
		GotoXY(x, y);
		std::cout << mark;
	}

	static void Table()
	{
		std::cout << "  x| 0  1  2->      " << std::endl;
		std::cout << "y__|_________       " << std::endl;
		std::cout << "  0|   | |          " << std::endl;
		std::cout << "   |---------       " << std::endl;
		std::cout << "  1|   | |          " << std::endl;
		std::cout << "   |---------       " << std::endl;
		std::cout << "  2|   | |          " << std::endl;
		std::cout << "   ``````````       " << std::endl;
		std::cout << "Press \"b\" to back!Like \"x = b\" or \"y = B\"\n";
	}
};

class Receiver
{
public://0 == ' ' 1 == 'X' 2 == 'O'
	void Input(int x, int y, int mark, std::vector<std::vector<int>> &table)
	{
		table[x][y] = mark;
		mark == 0 ? Display::Out_Result(x, y, ' ') : mark == 1 ? Display::Out_Result(x, y, 'X') : Display::Out_Result(x, y, 'O');
	}
};

class ICommand abstract
{
protected:
	std::vector<std::vector<int>> table = { {0,0,0}, {0,0,0}, {0,0,0} };
	int mark;
	Receiver receiver;
	int x, y;
public:
	virtual void Execute() abstract;
	virtual void UnExecute() abstract;
	std::vector<std::vector<int>> GetTable()
	{
		return table;
	}
};

class Command : public ICommand
{
public:
	Command(int current_mark, int c_x, int c_y, std::vector<std::vector<int>> c_table)
	{
		mark = current_mark;
		x = c_x;
		y = c_y;
		table = c_table;
	}

	void Execute() override
	{
		receiver.Input(x, y, mark, table);
	}

	void UnExecute() override
	{
		receiver.Input(x, y, 0, table);
	}
};

class Invoker
{
	std::vector<ICommand*> commands;
	int current_command;
public:
	std::vector<std::vector<int>> Undo()
	{
		if (current_command > 0)
		{
			commands[--current_command]->UnExecute();
			commands.pop_back();
		}
		if (current_command == 0)
		{
			return { {0,0,0},{0,0,0},{0,0,0} };
		}
		return commands[current_command - 1]->GetTable();
	}
	std::vector<std::vector<int>> Push(ICommand *Com)
	{
		Com->Execute();
		commands.push_back(Com);
		current_command++;
		return Com->GetTable();
	}
};

class Game
{
	std::vector<std::vector<int>> table = { {0,0,0}, {0,0,0}, {0,0,0} };
	ICommand *com;
	Invoker call;

	bool check_gorizontal()
	{
		auto count_x = 0, count_o = 0;
		for (auto j = 0; j < 3; j++)
		{
			count_x = 0;
			count_o = 0;
			for (auto i = 0; i < 3; i++)
				table[i][j] == 1 ? count_x++ : table[i][j] == 2 ? count_o++ : count_o;
			if (count_x == 3 || count_o == 3)
				return true;
		}
		return false;
	}

	bool check_vertical()
	{
		auto count_x = 0, count_o = 0;
		for (auto j = 0; j < 3; j++)
		{
			count_x = 0;
			count_o = 0;
			for (auto i = 0; i < 3; i++)
				table[j][i] == 1 ? count_x++ : table[j][i] == 2 ? count_o++ : count_o;
			if (count_x == 3 || count_o == 3)
				return true;
		}
		return false;
	}

	bool check_diagonals()
	{
		auto count_x = 0, count_o = 0;
		for (auto i = 0, j = 0; i < 3; i++, j++)
			table[i][j] == 1 ? count_x++ : table[i][j] == 2 ? count_o++ : count_o;
		if (count_x == 3 || count_o == 3)
			return true;

		count_x = 0; count_o = 0;
		for (auto i = 2, j = 0; j < 3; i--, j++)
			table[i][j] == 1 ? count_x++ : table[i][j] == 2 ? count_o++ : count_o;

		return count_x == 3 || count_o == 3 ? true : false;
	}

	bool Draw()
	{
		auto count = 0;
		for (auto i = 0; i < 3; i++)
			for (auto j = 0; j < 3; j++)
				if (table[i][j])
					count++;
		if (count == 9)
		{
			std::cout << "DRAW!!\n";
			return true;
		}
		return false;
	}
public:
	bool check_victory()
	{
		return check_diagonals() || check_gorizontal() || check_vertical() || Draw();
	}

	bool input(int &step)
	{
		std::string x = "", y = "";
		do
		{
			Display::Clear();
			Display::GotoXY(2, 9);
			std::cout << "x = ";
			getline(std::cin, x);
			Display::GotoXY(8, 9);
			std::cout << "y = ";
			getline(std::cin, y);
		} while (!std::regex_match(x, std::regex("^([{0-2}]|b){1}")) || !std::regex_match(y, std::regex("^([{0-2}]|b){1}")));

		if (x == "b" || y == "b" || x == "B" || y == "B")
			return false;

		if (!table[atoi(x.c_str())][atoi(y.c_str())])
			table = call.Push((com = new Command((step % 2 == 1) ? 1 : 2, atoi(x.c_str()), atoi(y.c_str()), table)));
		else
			step--;
		return true;
	}

	void back()
	{
		table = call.Undo();
	}
};

int main()
{
	Display::Table();
	Game game;
	auto i = 0;
	do
	{
		i++;
		if (!game.input(i) && i > 1)
			game.back();
	} while (!game.check_victory());
	system("pause");
	return false;
}