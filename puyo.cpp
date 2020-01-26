//課題1
//2019/04/12
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include<cmath>
#include <set>
#include <vector>
using namespace std;

//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };

puyocolor puyoOrder[3][2];

void init_puyo(){
	puyoOrder[0][0] = static_cast<puyocolor>(rand() % 4 + 1);
	puyoOrder[0][1] = static_cast<puyocolor>(rand() % 4 + 1);

	puyoOrder[1][0] = static_cast<puyocolor>(rand() % 4 + 1);
	puyoOrder[1][1] = static_cast<puyocolor>(rand() % 4 + 1);

}




class PuyoArray{
private:
	puyocolor *data;
	//盤面の行数，列数
	unsigned int data_line;
	unsigned int data_column;

public:
	PuyoArray() {
		data = NULL;
		//盤面の行数，列数
		data_line = 0;
		data_column = 0;
	}

	~PuyoArray(){
		Release();
	}
private:

	void Release()
	{
		if (data == NULL) {
			return;
		}
		delete[] data;
		data = NULL;
	}


public:
	void ChangeSize(unsigned int line, unsigned int column)
	{
		Release();

		//新しいサイズでメモリ確保
		data = new puyocolor[line*column];

		data_line = line;
		data_column = column;
	}

	//盤面の行数を返す
	unsigned int GetLine()
	{
		return data_line;
	}

	//盤面の列数を返す
	unsigned int GetColumn()
	{
		return data_column;
	}

	//盤面の指定された位置の値を返す
	puyocolor GetValue(unsigned int y, unsigned int x)
	{
		if (y >= GetLine() || x >= GetColumn())
		{
			//引数の値が正しくない
			return NONE;
		}

		return data[y*GetColumn() + x];
	}

	//盤面の指定された位置に値を書き込む
	void SetValue(unsigned int y, unsigned int x, puyocolor value)
	{
		if (y >= GetLine() || x >= GetColumn())
		{
			//引数の値が正しくない
			return;
		}

		data[y*GetColumn() + x] = value;
	}

};

class PuyoArrayActive:public PuyoArray{
public:
	PuyoArrayActive(){
		puyorotate = 0;
	}
	void SetState(int _puyorotate)
	{
		puyorotate = _puyorotate;
	}

	int GetState()
	{
		return puyorotate;
	}

private:
	int puyorotate;
};

class PuyoArrayStack:public PuyoArray{
};

void StackPuyo(int y, int x, PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
{
	if(apuyo.GetValue(y, x) != NONE)
	{
		spuyo.SetValue(y,x,apuyo.GetValue(y,x));
		apuyo.SetValue(y,x,NONE);
	}
}

void StackPuyo(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
{
	for (int y = 0; y < apuyo.GetLine(); y++)
	{
		for (int x = 0; x < apuyo.GetColumn(); x++)
		{
			StackPuyo(y,x,spuyo,apuyo);
		}
	}
}



puyocolor IntegratedValue(int y, int x, PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)//(x,y)の座標にあるpuyocolorを返す
{
	if(spuyo.GetValue(y,x) != NONE){
		return spuyo.GetValue(y,x);
	}
	if(apuyo.GetValue(y,x) != NONE){
		return apuyo.GetValue(y,x);
	}
	if(spuyo.GetValue(y,x) == NONE && apuyo.GetValue(y,x) == NONE){
		return NONE;
	}
}

void DisplayLandPoint(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
{
	int xpuyol;//ぷよのあるx座標
	int ypuyol;

	bool apuyo_is_exist = false;

	for (int y = apuyo.GetLine() -1; y >=0; y--)
	{
		for (int x = apuyo.GetColumn()-1 ; x >= 0; x--)
		{
			if(apuyo.GetValue(y,x) != NONE)
			{
				apuyo_is_exist = true;

				for (int _y = apuyo.GetLine() -1;_y >=0;_y--)
				{
					if (IntegratedValue(_y,x,spuyo,apuyo) == NONE && y < _y)
					{

						switch (apuyo.GetValue(y,x))
						{
							case NONE:
							attrset(COLOR_PAIR(0));
							mvaddch(_y, x, '*');
							break;
							case RED:
							attrset(COLOR_PAIR(4));
							mvaddch(_y, x, '*');
							break;
							case BLUE:
							attrset(COLOR_PAIR(1));
							mvaddch(_y, x, '*');
							break;
							case GREEN:
							attrset(COLOR_PAIR(2));
							mvaddch(_y, x, '*');
							break;
							case YELLOW:
							attrset(COLOR_PAIR(3));
							mvaddch(_y, x, '*');
							break;
							default:
							mvaddch(_y, x, '*');
							break;
						}
						break;
					}
				}
			}
		}
	}
	refresh();
}

void Display(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
{
	//落下中ぷよ表示
	for (int y = 0; y < apuyo.GetLine(); y++)
	{
		for (int x = 0; x < apuyo.GetColumn(); x++)
		{
			switch (IntegratedValue(y,x,spuyo,apuyo))
			{
				case NONE:
				attrset(COLOR_PAIR(0));
				mvaddch(y, x, '.');
				break;
				case RED:
				attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'R');
				break;
				case BLUE:
				attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'B');
				break;
				case GREEN:
				attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'G');
				break;
				case YELLOW:
				attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'Y');
				break;
				default:
				mvaddch(y, x, '?');
				break;
			}

		}
	}

	DisplayLandPoint(spuyo,apuyo);


	//情報表示
	int count = 0;
	for (int y = 0; y < apuyo.GetLine(); y++)
	{
		for (int x = 0; x < apuyo.GetColumn(); x++)
		{
			if (IntegratedValue(y,x,spuyo,apuyo) != NONE)
			{
				count++;
			}
		}
	}

	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", apuyo.GetLine(), apuyo.GetColumn(), count);
	attrset(COLOR_PAIR(0));
	mvaddstr(2, COLS - 35, msg);

	refresh();
}



bool is_NONE(int y, int x, PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
{
	if(x < 0 || y < 0)
	{
		return false;
	}
	if(x >= spuyo.GetColumn() || y >= spuyo.GetLine())
	{
		return false;
	}

	if(IntegratedValue(y,x,spuyo,apuyo) == NONE)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool ctrlable(PuyoArrayActive &apuyo)
{
	for (int y = 0; y < apuyo.GetLine(); y++)
	{
		for (int x = 0; x < apuyo.GetColumn(); x++)
		{
			if (apuyo.GetValue(y,x) != NONE)
			{
				if(apuyo.GetValue(y,x+1) != NONE || apuyo.GetValue(y+1,x) !=NONE)
				{
					return true;
				}
			}
		}
	}
}

void DisplayNextPuyo()
{
	char msg[256];
	sprintf(msg, "*--*");
	attrset(COLOR_PAIR(0));
	for(int i=0;i<4;i++){
		mvaddstr(1+i*2, COLS - 39, msg);
	}
	for(int i=0;i<3;i++){
		mvaddch(2+i*2, COLS - 39, '|');
	}
	for(int i=0;i<3;i++){
		mvaddch(2+i*2, COLS - 36, '|');
	}



	for(int i=0;i<3;i++)
	{
		for(int j=0;j<2;j++)
		{

			switch (puyoOrder[i][j])
			{
				case RED:
				attrset(COLOR_PAIR(4));
				mvaddch(2+i*2, COLS - 38 + j, 'R');
				break;
				case BLUE:
				attrset(COLOR_PAIR(1));
				mvaddch(2+i*2, COLS - 38 + j, 'B');
				break;
				case GREEN:
				attrset(COLOR_PAIR(2));
				mvaddch(2+i*2, COLS - 38 + j, 'G');
				break;
				case YELLOW:
				attrset(COLOR_PAIR(3));
				mvaddch(2+i*2, COLS - 38 + j, 'Y');
				break;
				default:
				mvaddch(2+i*2, COLS - 38 + j, '?');
				break;
			}
		}
	}




}

//盤面サイズ変更
class PuyoControl{
private:


public:

	//ぷよ消滅処理を全座標で行う
	//消滅したぷよの数を返す
	int VanishPuyo(PuyoArrayStack &puyostack, int &colornum, vector<int>& linked)
	{
		std::set<puyocolor> pc;
		int vanishednumber = 0;
		int color_num = 0;
		for (int y = 0; y < puyostack.GetLine(); y++)
		{
			for (int x = 0; x < puyostack.GetColumn(); x++)
			{
				int c = VanishPuyo(puyostack, y, x);

				vanishednumber += c;

				linked.push_back(c);

				if(c != 0)
				{
					pc.insert(puyostack.GetValue(y,x));
				}

			}
		}

		colornum = pc.size();

		return vanishednumber;
	}

	//ぷよ消滅処理を座標(x,y)で行う
	//消滅したぷよの数を返す
	int VanishPuyo(PuyoArrayStack &puyostack, unsigned int y, unsigned int x)
	{
		//判定個所にぷよがなければ処理終了
		if (puyostack.GetValue(y, x) == NONE)
		{
			return 0;
		}


		//判定状態を表す列挙型
		//NOCHECK判定未実施，CHECKINGが判定対象，CHECKEDが判定済み
		enum checkstate{ NOCHECK, CHECKING, CHECKED };

		//判定結果格納用の配列
		enum checkstate *field_array_check;
		field_array_check = new enum checkstate[puyostack.GetLine()*puyostack.GetColumn()];

		//配列初期化
		for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++)
		{
			field_array_check[i] = NOCHECK;
		}

		//座標(x,y)を判定対象にする
		field_array_check[y*puyostack.GetColumn() + x] = CHECKING;

		//判定対象が1つもなくなるまで，判定対象の上下左右に同じ色のぷよがあるか確認し，あれば新たな判定対象にする
		bool checkagain = true;
		while (checkagain)
		{
			checkagain = false;

			for (int y = 0; y < puyostack.GetLine(); y++)
			{
				for (int x = 0; x < puyostack.GetColumn(); x++)
				{
					//(x,y)に判定対象がある場合
					if (field_array_check[y*puyostack.GetColumn() + x] == CHECKING)
					{
						//(x+1,y)の判定
						if (x < puyostack.GetColumn() - 1)
						{
							//(x+1,y)と(x,y)のぷよの色が同じで，(x+1,y)のぷよが判定未実施か確認
							if (puyostack.GetValue(y, x + 1) == puyostack.GetValue(y, x) && field_array_check[y*puyostack.GetColumn() + (x + 1)] == NOCHECK)
							{
								//(x+1,y)を判定対象にする
								field_array_check[y*puyostack.GetColumn() + (x + 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(x-1,y)の判定
						if (x > 0)
						{
							if (puyostack.GetValue(y, x - 1) == puyostack.GetValue(y, x) && field_array_check[y*puyostack.GetColumn() + (x - 1)] == NOCHECK)
							{
								field_array_check[y*puyostack.GetColumn() + (x - 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(x,y+1)の判定
						if (y < puyostack.GetLine() - 1)
						{
							if (puyostack.GetValue(y + 1, x) == puyostack.GetValue(y, x) && field_array_check[(y + 1)*puyostack.GetColumn() + x] == NOCHECK)
							{
								field_array_check[(y + 1)*puyostack.GetColumn() + x] = CHECKING;
								checkagain = true;
							}
						}

						//(x,y-1)の判定
						if (y > 0)
						{
							if (puyostack.GetValue(y - 1, x) == puyostack.GetValue(y, x) && field_array_check[(y - 1)*puyostack.GetColumn() + x] == NOCHECK)
							{
								field_array_check[(y - 1)*puyostack.GetColumn() + x] = CHECKING;
								checkagain = true;
							}
						}

						//(x,y)を判定済みにする
						field_array_check[y*puyostack.GetColumn() + x] = CHECKED;
					}
				}
			}
		}

		//判定済みの数をカウント
		int puyocount = 0;
		for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++)
		{
			if (field_array_check[i] == CHECKED)
			{
				puyocount++;
			}
		}

		int millisecond = 1*100000;



		//消えるぷよを点滅させたかったのでここで点滅させた。
		int blnkc = 0;
		if(puyocount >=4)
		{

			while(blnkc != 4)
			{
				for (int y = 0; y < puyostack.GetLine(); y++)
				{
					for (int x = 0; x < puyostack.GetColumn(); x++)
					{
						if (field_array_check[y*puyostack.GetColumn() + x] == CHECKED)
						{
							mvaddch(y,x,' ');
							refresh();
						}

					}
				}

				usleep(millisecond);
				for (int y = 0; y < puyostack.GetLine(); y++)
				{
					for (int x = 0; x < puyostack.GetColumn(); x++)
					{
						if (field_array_check[y*puyostack.GetColumn() + x] == CHECKED)
						{
							switch (puyostack.GetValue(y,x))
							{
								case NONE:
								attrset(COLOR_PAIR(0));
								mvaddch(y, x, '.');
								break;
								case RED:
								attrset(COLOR_PAIR(4));
								mvaddch(y, x, 'R');
								break;
								case BLUE:
								attrset(COLOR_PAIR(1));
								mvaddch(y, x, 'B');
								break;
								case GREEN:
								attrset(COLOR_PAIR(2));
								mvaddch(y, x, 'G');
								break;
								case YELLOW:
								attrset(COLOR_PAIR(3));
								mvaddch(y, x, 'Y');
								break;
								default:
								mvaddch(y, x, '?');
								break;
							}
							refresh();
						}
					}
				}
				usleep(millisecond);

				blnkc++;


			}
		}


		//4個以上あれば，判定済み座標のぷよを消す
		int vanishednumber = 0;






		if (4 <= puyocount)
		{
			for (int y = 0; y < puyostack.GetLine(); y++)
			{
				for (int x = 0; x < puyostack.GetColumn(); x++)
				{
					if (field_array_check[y*puyostack.GetColumn() + x] == CHECKED)
					{
						puyostack.SetValue(y, x, NONE);

						vanishednumber++;
					}
				}
			}
		}



		//メモリ解放
		delete[] field_array_check;

		return vanishednumber;
	}

	void speedup(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		for (int y = apuyo.GetLine() -1; y >= 0; y--)
		{
			for (int x = 0; x < apuyo.GetColumn(); x++)
			{
				if(apuyo.GetValue(y,x) != NONE && y < apuyo.GetLine()-1)
				{

					if(!is_NONE(y+1,x,spuyo,apuyo))
					{
						return;
					}

					if(spuyo.GetValue(y+1,x) == NONE)
					{
						apuyo.SetValue(y+1,x,apuyo.GetValue(y,x));
						apuyo.SetValue(y,x,NONE);
					}

				}
			}
		}
	}


//盤面に新しいぷよ生成
	void GeneratePuyo(PuyoArrayActive &apuyo)
	{
		init_pair(0, COLOR_WHITE, COLOR_BLACK);
		init_pair(1, COLOR_BLUE, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_YELLOW, COLOR_BLACK);
		init_pair(4, COLOR_RED, COLOR_BLACK);

		apuyo.SetValue(0, 5, puyoOrder[0][0]);
		apuyo.SetValue(0, 6, puyoOrder[0][1]);

		puyoOrder[0][0] = puyoOrder[1][0];
		puyoOrder[0][1] = puyoOrder[1][1];

		puyoOrder[1][0] = puyoOrder[2][0];
		puyoOrder[1][1] = puyoOrder[2][1];

		puyoOrder[2][0] = static_cast<puyocolor>(rand() % 4 + 1);
		puyoOrder[2][1] = static_cast<puyocolor>(rand() % 4 + 1);
	}


	//ぷよの着地判定．着地判定があるとtrueを返す
	bool LandingPuyo(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		bool landed = true;


		for (int y = 0; y < apuyo.GetLine(); y++)
		{
			for (int x = 0; x < apuyo.GetColumn(); x++)
			{
				if (IntegratedValue(y, x, spuyo, apuyo) != NONE && y < apuyo.GetLine()-1)//もし、最後の行以外にぷよがあれば内部のif文が読まれる
				{


					if (IntegratedValue(y+1, x, spuyo, apuyo) == NONE)											//(最後の行にない)ぷよの下にぷよがなければlandedはfalseでこの関数は終了する
					{
						return false;
					}
					else									//もし、すべてのぷよの下にぷよがあればlandedはずっとtrueのまま。
					{
						landed = true;
					}
				}
			}
		}

		return landed;
	}

	//左移動
	void MoveLeft(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		//1つ左の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = apuyo.GetLine() -1; y >=0; y--)
		{
			for (int x = 0; x < apuyo.GetColumn(); x++)
			{
				if(apuyo.GetValue(y, x) != NONE)//x=0のときは左に動かせないので同じ座標をtempに入力
				{
					if(x == 0)//左にぷよがあったら左に動かせないので同じ座標をtempに入力(ここにx=0はいれられない)
					{
						return;
					}
					if (spuyo.GetValue(y, x - 1) == NONE)
					{
						apuyo.SetValue(y,x-1, apuyo.GetValue(y, x));
						apuyo.SetValue(y, x, NONE);
					}
					else
					{
						return;
					}
				}
			}
		}
	}

	//右移動
	void MoveRight(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		//1つ左の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = apuyo.GetLine() -1; y >=0; y--)
		{
			for (int x = apuyo.GetColumn()-1 ; x >= 0; x--)
			{
				if(apuyo.GetValue(y, x) != NONE)//x=0のときは左に動かせないので同じ座標をtempに入力
				{
					if(x == apuyo.GetColumn()-1)//左にぷよがあったら左に動かせないので同じ座標をtempに入力(ここにx=0はいれられない)
					{
						return;
					}
					if (spuyo.GetValue(y, x + 1) == NONE)
					{
						apuyo.SetValue(y,x+1, apuyo.GetValue(y, x));
						apuyo.SetValue(y, x, NONE);
					}
					else
					{
						return;
					}
				}
			}
		}
	}

	//下移動
	void MoveDown(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		//一時的格納場所メモリ確

		//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = spuyo.GetLine() - 2; y >= 0; y--)
		{
			for (int x = 0; x < spuyo.GetColumn(); x++)
			{
				//(y,x)になんかあったら次にすすめる。
				if (spuyo.GetValue(y, x) == NONE && apuyo.GetValue(y,x) == NONE)
				{
					continue;
				}

				//下にspuyoがなかったら、一個下に下がる。
				if (spuyo.GetValue(y + 1, x) == NONE)
				{
					if(apuyo.GetValue(y,x) != NONE)
					{
						apuyo.SetValue(y+1,x,apuyo.GetValue(y, x));
						//コピー後に元位置のpuyoactiveのデータは消す
						apuyo.SetValue(y, x, NONE);
					}
					if(spuyo.GetValue(y,x) != NONE)
					{
						spuyo.SetValue(y+1,x,spuyo.GetValue(y, x));
						//コピー後に元位置のpuyostackのデータは消す
						spuyo.SetValue(y, x, NONE);
					}
				}
			}
		}

		//puyo_tempからpuyoactiveへコピー

	}

	void Rotate(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		for (int y = 0; y < apuyo.GetLine()-1; y++)
		{
			for (int x = 0; x < apuyo.GetColumn()-1; x++)
			{

				if (apuyo.GetValue(y,x) != NONE)						//以下、(y,x)にactiveぷよがある場合。
				{

					//rotate 0
					if(IntegratedValue(y, x+1,spuyo,apuyo) != NONE && is_NONE(y+1,x,spuyo,apuyo))
					{
						if(apuyo.GetState() == 0)
						{
							apuyo.SetValue(y+1,x,apuyo.GetValue(y,x+1));
							apuyo.SetValue(y,x+1,NONE);
							apuyo.SetState(1);
							return;
						}
					}
					//rotate1
					if (apuyo.GetValue(y+1,x) != NONE && is_NONE(y,x-1,spuyo,apuyo))
					{
						if(apuyo.GetState() == 1)
						{
							apuyo.SetValue(y,x-1,apuyo.GetValue(y+1,x));
							apuyo.SetValue(y+1,x,NONE);
							apuyo.SetState(2);
							return;
						}
					}
					//rotate2
					if(IntegratedValue(y, x + 1,spuyo,apuyo) != NONE && apuyo.GetState() == 2)
					{
						apuyo.SetValue(y-1,x+1,apuyo.GetValue(y,x));
						apuyo.SetValue(y,x,NONE);
						apuyo.SetState(3);
						return;
					}
					//rotate3
					if(IntegratedValue(y+1,x,spuyo,apuyo) !=NONE && apuyo.GetState() == 3)
					{
						apuyo.SetValue(y+1,x+1,apuyo.GetValue(y,x));
						apuyo.SetValue(y,x,NONE);
						apuyo.SetState(0);
						return;
					}
				}

			}

		}

	}

	void RevRotate(PuyoArrayStack &spuyo, PuyoArrayActive &apuyo)
	{
		for (int y = 0; y < apuyo.GetLine()-1; y++)
		{
			for (int x = 0; x < apuyo.GetColumn()-1; x++)
			{

				if (apuyo.GetValue(y,x) != NONE)						//以下、(y,x)にactiveぷよがある場合。
				{

					//rotate 0
					if(IntegratedValue(y, x+1,spuyo,apuyo) != NONE &&apuyo.GetState() == 0)//０の状態であることを確認。また移動後の場所が存在するか確認。
					{

						apuyo.SetValue(y-1,x,apuyo.GetValue(y,x+1));
						apuyo.SetValue(y,x+1,NONE);
						apuyo.SetState(3);
						return;

					}
					//rotate1
					if (apuyo.GetValue(y+1,x) != NONE && is_NONE(y,x+1,spuyo,apuyo))
					{
						if(apuyo.GetState() == 1)
						{
							apuyo.SetValue(y,x+1,apuyo.GetValue(y+1,x));
							apuyo.SetValue(y+1,x,NONE);
							apuyo.SetState(0);
							return;
						}
					}
					//rotate2
					if(IntegratedValue(y, x+1,spuyo,apuyo) != NONE && apuyo.GetState() == 2)
					{
						if(is_NONE(y+1,x+1,spuyo,apuyo))
						apuyo.SetValue(y+1,x+1,apuyo.GetValue(y,x));
						apuyo.SetValue(y,x,NONE);
						apuyo.SetState(1);
						return;
					}
					//rotate3
					if(IntegratedValue(y+1,x,spuyo,apuyo) !=NONE && apuyo.GetState() == 3)
					{
						if(is_NONE(y+1,x-1,spuyo,apuyo))
						{
							apuyo.SetValue(y+1,x-1,apuyo.GetValue(y,x));
							apuyo.SetValue(y,x,NONE);
							apuyo.SetState(2);
							return;

						}
					}
				}
			}
		}
	}
};

int PointCalc(int vanish,int chain, vector<int> &linked, int cnum)
{
	int tmp_point = 0;
	int flag;
	if(1<chain && chain < 5)
	{

		tmp_point += pow(2,chain+1);
	}
	else if(chain > 4 && chain < 20)
	{
		tmp_point += 32*(chain-3);
	}else if(19 < chain){
		tmp_point += 512;
	}
	for (int i=0;i<linked.size();i++){
		if(linked[i]>4 && linked[i] < 11)
		{
			tmp_point += linked[i] - 3;
			flag = 1;
		}
		else if(linked[i] > 10){
			tmp_point += 10;
			flag = 1;
		}else{
			flag = 0;
		}
	}

	if (vanish == 4 && chain == 1)
	{
		if(cnum == 1 && flag ==0){
			return 40;
		}
	}
	if(1 < cnum && cnum < 6){

		tmp_point += 3*pow(2,cnum-2);
	}

	return vanish * tmp_point*10;
}




//ここから実行される
int main(int argc, char **argv){
	PuyoArrayActive apuyo;
	PuyoArrayStack spuyo;
	PuyoControl ctrl;
	//画面の初期化

	initscr();
	//カラー属性を扱うための初期化
	start_color();

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);


	//初期化処理
	apuyo.ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	spuyo.ChangeSize(LINES/2, COLS/2);
	ctrl.GeneratePuyo(apuyo);	//最初のぷよ生成



	int point = 0;
	int delay = 0;
	int delay2 = 0;
	int waitCount = 10000;
	int waitCount2 = 1000;
	int vanish;
	int chain;
	int colornum;
	vector<int> linked;
	char msg[256];
	char msg2[256];
	sprintf(msg2, "z:rotate x:reverse KEY_DOWN:speedup");
	mvaddstr(10, COLS-39, msg2);
	refresh();





	//メイン処理ループ
	while (1)//activepuyoが上からしたに降りるまでで１ループ
	{
		//キー入力受付
		int tmp_point = 0;
		int ch;
		waitCount = 10000;
		chain = 0;
		ch = getch();
		colornum = 0;

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}



		//入力キーごとの処理
		if(ctrlable(apuyo))
		{
			switch (ch)
			{
				case KEY_LEFT:
				ctrl.MoveLeft(spuyo, apuyo);
				break;
				case KEY_RIGHT:
				ctrl.MoveRight(spuyo, apuyo);
				break;
				case KEY_DOWN:
				ctrl.speedup(spuyo,apuyo);
				break;
				case 'z':
				ctrl.Rotate(spuyo, apuyo);
				break;
				case 'x':
				ctrl.RevRotate(spuyo, apuyo);
				break;
				default:
				break;
			}

		}
		else
		{
			waitCount = 1000;
		}



		//処理速度調整のためのif文
		if (delay%waitCount == 0)
		{


			//ぷよ着地判定
			if (ctrl.LandingPuyo(spuyo,apuyo))
			{

				vanish = 1;																//最初はwhile文を通らせたいので、１にする。
				StackPuyo(spuyo,apuyo);										//puyoをアクティブからスタック状態にする。

				while(vanish != 0)								//連鎖
				{
					if(chain >= 2)//最後ににvanish=０で判定するので　chain++;　実行。
					{
						sprintf(msg, "%d chain!!!",chain);
						mvaddstr(4, COLS - 35, msg);//表示時間がdelay2に依存
						refresh();
					}

					vanish = ctrl.VanishPuyo(spuyo, colornum,linked);//消す（消せなかったら、vanish = 0で終わり。）
					chain++;
					point += PointCalc(vanish,chain,linked,colornum);
					sprintf(msg, "Point: %d",point);
					mvaddstr(5, COLS - 35, msg);
					refresh();


					while(!ctrl.LandingPuyo(spuyo,apuyo))//Landしていない限り、
					{
						if (delay2%waitCount2 == 0)
						{
							ctrl.MoveDown(spuyo, apuyo);
						}
						delay2++;

						Display(spuyo,apuyo);
					}//Landしたら、

					StackPuyo(spuyo, apuyo);//スタック状態にする

				}
				linked.clear();
				linked.shrink_to_fit();

				apuyo.SetState(0);//全部着地したので、次のgenerateぷよに備える
								//全部着地していたら新しいぷよ生成

				refresh();
				ctrl.GeneratePuyo(apuyo);
				DisplayNextPuyo();
			}
			ctrl.MoveDown(spuyo, apuyo);
		}
		delay++;

		if(spuyo.GetValue(0,5) != NONE || spuyo.GetValue(0,6) != NONE)
		{
			sprintf(msg, "G A M E O V E R");
			mvaddstr(LINES/4, COLS/4, msg);
			refresh();
			sleep(4);
			sprintf(msg, "Press 'c' to restart or 'q' to quit");
			mvaddstr(LINES/4 + 1, COLS/4, msg);
			sprintf(msg, "Othewise, this game end in 10 sec");
			mvaddstr(LINES/4 + 2, COLS/4, msg);
			refresh();

			if(ch == 'c')
			{
				for (int y = 0; y < apuyo.GetLine(); y++)
				{
					for (int x = 0; x < apuyo.GetColumn(); x++)
					{
						apuyo.SetValue(x,y,NONE);
						spuyo.SetValue(x,y,NONE);
					}
				}
			}
			if(ch == 'q')
			{
				break;
			}
			sleep(10);
			break;
		}
		//表示
		Display(spuyo, apuyo);
	}
	//画面をリセット
	endwin();

	return 0;
}
