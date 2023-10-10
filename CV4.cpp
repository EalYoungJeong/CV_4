#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;
#define WIDTH 396	//가로길이
#define HEIGHT 400	//세로길이
#define img_size WIDTH*HEIGHT	//이미지 크기
#define HIGH_th 150	// high threshold
#define LOW_th 50	//low threshold

void padding_g(unsigned char* src, unsigned char* dest)
{//가우시안 필터(5x5) 적용을 위한 원본 이미지 same padding
	unsigned char first_c;
	unsigned char last_c;
	int padded_idx = 0;
	//상위 2줄 same padding
	dest[padded_idx++] = src[0];
	dest[padded_idx++] = src[0];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[i];
	dest[padded_idx++] = src[WIDTH - 1];
	dest[padded_idx++] = src[WIDTH - 1];
	dest[padded_idx++] = src[0];
	dest[padded_idx++] = src[0];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[i];
	dest[padded_idx++] = src[WIDTH - 1];
	dest[padded_idx++] = src[WIDTH - 1];

	//중간 400줄 same padding
	for (int m = 0; m < HEIGHT; m++)
	{
		first_c = src[m * WIDTH];
		dest[padded_idx++] = first_c;
		dest[padded_idx++] = first_c;
		for (int i = 0; i < WIDTH; i++)
			dest[padded_idx++] = src[m * WIDTH + i];
		last_c = src[(m + 1) * WIDTH - 1];
		dest[padded_idx++] = last_c;
		dest[padded_idx++] = last_c;
	}

	//하위 2줄 same padding
	dest[padded_idx++] = src[WIDTH * (HEIGHT - 1)];
	dest[padded_idx++] = src[WIDTH * (HEIGHT - 1)];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[WIDTH * (HEIGHT - 1) + i];
	dest[padded_idx++] = src[WIDTH * HEIGHT - 1];
	dest[padded_idx++] = src[WIDTH * HEIGHT - 1];
	dest[padded_idx++] = src[WIDTH * (HEIGHT - 1)];
	dest[padded_idx++] = src[WIDTH * (HEIGHT - 1)];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[WIDTH * (HEIGHT - 1) + i];
	dest[padded_idx++] = src[WIDTH * HEIGHT - 1];
	dest[padded_idx++] = src[WIDTH * HEIGHT - 1];
}

void padding_s(unsigned char* src, unsigned char* dest)
{//소벨  마스크(3x3) 적용을 위한 원본 이미지 same padding
	unsigned char first_c;
	unsigned char last_c;
	int padded_idx = 0;
	//상위 1줄 same padding
	dest[padded_idx++] = src[0];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[i];
	dest[padded_idx++] = src[WIDTH - 1];

	//중간 400줄 same padding
	for (int m = 0; m < HEIGHT; m++)
	{
		first_c = src[m * WIDTH];
		dest[padded_idx++] = first_c;
		for (int i = 0; i < WIDTH; i++)
			dest[padded_idx++] = src[m * WIDTH + i];
		last_c = src[(m + 1) * WIDTH - 1];
		dest[padded_idx++] = last_c;
	}

	//하위 1줄 same padding
	dest[padded_idx++] = src[WIDTH * (HEIGHT - 1)];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[WIDTH * (HEIGHT - 1) + i];
	dest[padded_idx++] = src[WIDTH * HEIGHT - 1];
}
void blurring(unsigned char* src, unsigned char* dest)//가우시안 필터 적용
{
	//가우시안 마스크
	double GaussianMask[5][5] = { {2,4,5,4,2},
								{4,9,12,9,4},
								{5,12,15,12,5},
								{4,9,12,9,4},
								{2,4,5,4,2} };
	int temp_sum = 0;

	//각 픽셀 별 convolution 연산
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			temp_sum = 0;
			for (int m = 0; m < 5; m++)
			{
				for (int n = 0; n < 5; n++)
				{
					temp_sum = temp_sum + src[((i + m) * 400) + (j + n)] * GaussianMask[m][n];
				}
			}
			dest[i * WIDTH + j] = temp_sum / 159;//값 정규화 및 픽셀 배열에 저장
		}
	}
}
void Getsobel(unsigned char* src, int* sobelX, int* sobelY)//소벨 필터 적용 및 X, Y Gradient 추출
{
	int Sobel_x[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1} };//X 소벨 마스크
	int Sobel_y[3][3] = { {1,2,1},{0,0,0},{-1,-2,-1} };//Y 소벨 마스크
	int temp_sum = 0;

	//convolution 연산
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			temp_sum = 0;
			for (int m = 0; m < 3; m++)
			{
				for (int n = 0; n < 3; n++)
					temp_sum = temp_sum + src[((i + m) * 398) + (j + n)] * Sobel_x[m][n];
			}
			sobelX[i * WIDTH + j] = temp_sum;

			temp_sum = 0;
			for (int m = 0; m < 3; m++)
			{
				for (int n = 0; n < 3; n++)
					temp_sum = temp_sum + src[((i + m) * 398) + (j + n)] * Sobel_y[m][n];
			}
			sobelY[i * WIDTH + j] = temp_sum;
		}
	}
}
void GetGradient_n_Angle(int* gradient_X, int* gradient_Y, unsigned char* gradient, int* angle, int* trueangle)//Gradient, angle, True angle 추출
{
	double temp_angle;
	const double PI = 3.141592;//파이
	for (int i = 0; i < img_size; i++)
	{
		gradient[i] = sqrt((gradient_X[i] * gradient_X[i]) + (gradient_Y[i] * gradient_Y[i]));//피타고라스 정리에 의한 Gradient 크기

		temp_angle = atan2(gradient_X[i], gradient_Y[i]) / PI * 180;//arctan 함수로 X와 Y 비에 대한 각도 theta 구하기, 180/PI => 라디안 변화
		trueangle[i] = temp_angle;//정규화 하지 않은 각도 저장

		//각도 정규화 0, 45, 90, 135 간격
		if (temp_angle < 22.5 && temp_angle > -22.5)
			temp_angle = 0;
		if (temp_angle < -157.5 || temp_angle > 157.5)
			temp_angle = 0;
		if (temp_angle < 67.5 && temp_angle > 22.5)
			temp_angle = 45;
		if (temp_angle < -112.5 && temp_angle > -157.5)
			temp_angle = 45;
		if (temp_angle < 112.5 && temp_angle > 67.5)
			temp_angle = 90;
		if (temp_angle < -67.5 && temp_angle > -112.5)
			temp_angle = 90;
		if (temp_angle < 157.5 && temp_angle > 112.5)
			temp_angle = 135;
		if (temp_angle < -22.5 && temp_angle > -67.5)
			temp_angle = 135;
		angle[i] = temp_angle;//정규화된 각도 저장
	}
}
void Non_maximum_suppression(unsigned char* gradient, int* angle, unsigned char* edge)//경계선 검출
{
	bool isMax;//주변 픽셀 중 가장 큰지 확인하는 변수
	memset(edge, 0, sizeof(unsigned char) * img_size);

	for (int i = 1; i < HEIGHT - 1; i++)
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			int cur_idx = (i * WIDTH) + j;//1차원 -> 2차원 매핑
			isMax = false;
			switch (angle[cur_idx])//정규화 각도에 따라
			{//해당 방향의 직선 내의 픽셀과 비교하여 자신이 가장 큰 값일 경우 true 반환
			case 0:
				if (gradient[cur_idx] >= gradient[cur_idx + 1] && gradient[cur_idx] >= gradient[cur_idx - 1])
					isMax = true;
				break;
			case 45:
				if (gradient[cur_idx] >= gradient[cur_idx + WIDTH + 1] && gradient[cur_idx] >= gradient[cur_idx - WIDTH - 1])
					isMax = true;
				break;
			case 90:
				if (gradient[cur_idx] >= gradient[cur_idx + WIDTH] && gradient[cur_idx] >= gradient[cur_idx - WIDTH])
					isMax = true;
				break;
			default:
				if (gradient[cur_idx] >= gradient[cur_idx + WIDTH - 1] && gradient[cur_idx] >= gradient[cur_idx - WIDTH + 1])
					isMax = true;
				break;
			}

			if (isMax)//만약 true일경우
			{
				if (gradient[cur_idx] > HIGH_th)//강한 경계선
					edge[cur_idx] = 255;
				else if (gradient[cur_idx] > LOW_th)//약한 경계선
					edge[cur_idx] = 128;
				else//제거
					edge[cur_idx] = 0;
			}
		}
	}
}

void edge_tracking(unsigned char* src)
{
	for (int i = 1; i < HEIGHT - 1; i++)//전체 픽셀 탐색
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			int cur_idx = (i * WIDTH) + j;
			if (src[cur_idx] == 255)//강한 경계선 기준에서 인접한 약한 경계선이 있을 경우
			{//약한 경계선 또한 강한 경계선으로 인정
				if (src[cur_idx + 1] == 128)
					src[cur_idx + 1] = 255;
				if (src[cur_idx - WIDTH + 1] == 128)
					src[cur_idx - WIDTH + 1] = 255;
				if (src[cur_idx - WIDTH] == 128)
					src[cur_idx - WIDTH] = 255;
				if (src[cur_idx - WIDTH - 1] == 128)
					src[cur_idx - WIDTH - 1] = 255;
				if (src[cur_idx - 1] == 128)
					src[cur_idx - 1] = 255;
				if (src[cur_idx + WIDTH - 1] == 128)
					src[cur_idx + WIDTH - 1] = 255;
				if (src[cur_idx + WIDTH] == 128)
					src[cur_idx + WIDTH] = 255;
				if (src[cur_idx + WIDTH + 1] == 128)
					src[cur_idx + WIDTH + 1] = 255;
			}
		}
	}

	//강한 경계선이 되지 못한 약한 경계선 모두 제거
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			int cur_idx = (i * WIDTH) + j;
			if (src[cur_idx] == 128)
				src[cur_idx] = 0;
		}
	}
}

void HoughTransform(unsigned char* edge, int* angle, int* TrueAngle)
{
	int* Acc = new int[img_size];//투표를 위한 누적 저장소
	for (int i = 0; i < img_size; i++)//초기화
		Acc[i] = 0;
	int MaxR = 100;//최대 반지름 100

	for (int i = 1; i < HEIGHT - 1; i++)
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			int cur_idx = (i * WIDTH) + j;//1차원 -> 2차원 매핑
			if (edge[cur_idx] == 255)//에지 내의 한 픽셀 기준
			{
				for (int r = 10; r < MaxR; r++)//픽셀 좌표 (i,j) 기준 반지금 길이 경우의 수
				{
					int th = angle[cur_idx];//현재 픽셀의 angle 정보
					//현재 픽셀의 angle 정보와 True angle이 거의 유사한 경우의 반지름 직선에만 투표
					if (th == 0 && (abs(TrueAngle[cur_idx]-th)<=3 || abs(TrueAngle[cur_idx]-th)>=177))//theta = 0 일때 직교하는 수직방향 투표
					{
						if (cur_idx + (r * WIDTH) < img_size && cur_idx + (r * WIDTH) >= 0)
							Acc[cur_idx + (r * WIDTH)]++;
						if (cur_idx - (r * WIDTH) < img_size && cur_idx - (r * WIDTH) >= 0)
							Acc[cur_idx - (r * WIDTH)]++;
					}
					if (th == 45 && abs(TrueAngle[cur_idx]-th)<=3)//theta = 45 일때 직교하는 135도 방향 투표
					{
						if (cur_idx + (r * WIDTH) - r >= 0 && cur_idx + (r * WIDTH) - r < img_size)
							Acc[cur_idx + (r * WIDTH) - r]++;
						if (cur_idx - (r * WIDTH) + r >= 0 && cur_idx - (r * WIDTH) + r < img_size)
							Acc[cur_idx - (r * WIDTH) + r]++;
					}
					if (th == 90 && abs(TrueAngle[cur_idx] - th) <= 3)//theta = 90 일때 직교하는 수평방향 투표
					{
						if (cur_idx + r < img_size && cur_idx + r >= 0)
							Acc[cur_idx + r]++;
						if (cur_idx - r < img_size && cur_idx - r >= 0)
							Acc[cur_idx - r]++;
					}
					if (th == 135 && abs(TrueAngle[cur_idx] - th) <= 3)//theta = 135 일때 직교하는 45도 방향 투표
					{
						if (cur_idx - (r * WIDTH) - r >= 0 && cur_idx - (r * WIDTH) - r < img_size)
							Acc[cur_idx - (r * WIDTH) - r]++;
						if (cur_idx + (r * WIDTH) + r >= 0 && cur_idx + (r * WIDTH) + r < img_size)
							Acc[cur_idx + (r * WIDTH) + r]++;
					}
				}
			}
		}
	}

	//반지름 확인용 테스트 이미지 출력
	FILE* wp = fopen("test.yuv", "wb");
	unsigned char* test = new unsigned char[img_size];
	int Ranking[4][3] = {0};
	
	//각 부분(좌상, 우상, 좌하, 우하)별 최대 투표수를 가진 픽셀 검출 
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			int cur_idx = (i * WIDTH) + j;
			if (Acc[cur_idx] > 0)//투표 어느정도 받은 픽셀 중
			{
				//cout << Acc[cur_idx] << " " << i << " " << j << endl;//후보 픽셀 출력
				test[cur_idx] = 255;//이미지에 출력

				//모든 픽셀 탐색하며 파트 별 최대값 최신화
				if (i < 200)
				{
					if (j < 196)//좌상
					{
						if (Ranking[0][2] <= Acc[cur_idx])
						{
							Ranking[0][2] = Acc[cur_idx];
							Ranking[0][0] = i;
							Ranking[0][1] = j;
						}
					}
					else//우상
					{
						if (Ranking[1][2] <= Acc[cur_idx])
						{
							Ranking[1][2] = Acc[cur_idx];
							Ranking[1][0] = i;
							Ranking[1][1] = j;
						}
					}
				}
				else
				{
					if (j < 196)//좌하
					{
						if (Ranking[2][2] <= Acc[cur_idx])
						{
							Ranking[2][2] = Acc[cur_idx];
							Ranking[2][0] = i;
							Ranking[2][1] = j;
						}
					}
					else//우하
					{
						if (Ranking[3][2] <= Acc[cur_idx])
						{
							Ranking[3][2] = Acc[cur_idx];
							Ranking[3][0] = i;
							Ranking[3][1] = j;
						}
					}
				}
			}
			else test[cur_idx] = 0;
		}
	}
	fwrite(test, sizeof(unsigned char), img_size, wp);
	cout << "각 구역별 최대 득표 좌표와 득표율" << endl;
	for (int i = 0; i < 4; i++)//각 부분 별 최대 득표 좌표 출력
	{
		cout << i + 1 <<". " << Ranking[i][0] << " " << Ranking[i][1] << "    " << Ranking[i][2] << endl;
	}

	cout << endl << endl;
	///구한 좌표를 기준으로 반지름 구하기
	for (int i = 0; i < 4; i++)
	{
		int cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//최대 득표 좌표의 1차원 매핑
		int dist = 0;//반지름 
		int dist_av = 0;//반지름 평균
		int dist_c = 0;//반지름 개수
		
		//현재 좌표를 기준으로 8방향에 대해 에지 검출 및 에지와의 거리 dist
		while (1)
		{//왼쪽
			if (edge[cur_idx--] == 255) break;
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

		
		while (1)
		{//오른쪽
			if (edge[cur_idx++] == 255) break;
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

		
		while (1)
		{//아래쪽
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx + WIDTH;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

		
		while (1)
		{//우하
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx + WIDTH;
				cur_idx = cur_idx + 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

		
		while (1)
		{//좌상
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx - WIDTH;
				cur_idx = cur_idx - 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

		
		while (1)
		{//우상
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx - WIDTH;
				cur_idx = cur_idx + 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

	
		while (1)
		{//좌하
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx + WIDTH;
				cur_idx = cur_idx - 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//에지를 찾은 경우
		{
			dist_av = dist_av + dist;//누적 덧셈
			dist_c++;//개수 증가
		}
		dist = 0;//거리 초기화
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//좌표 초기화

	
		while (1)
		{//위쪽
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx - WIDTH;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)
		{
			dist_av = dist_av + dist;
			dist_c++;
		}
		if (dist_c == 0)//에지를 하나도 검출 못한 경우 오류
		{
			cout << "오류" << endl;
			continue;
		}
		else//반지름 평균 결과 도출
			dist_av = dist_av / dist_c;

		cout << "a , b =  " << Ranking[i][1] << " , " << Ranking[i][0] << "    반지름 : " << dist_av << endl; //결과출력
	}
}

int main()
{
	unsigned char* arr_img = new unsigned char[img_size];//테스트 이미지
	unsigned char* Padded_img = new unsigned char[(WIDTH + 4) * (HEIGHT + 4)];//가우시안 필터를 위한 패딩
	unsigned char* Padded_img_s = new unsigned char[(WIDTH + 2) * (HEIGHT + 2)];//sobel 마스크 적용을 위한 패딩
	unsigned char* Blurred_img = new unsigned char[img_size];//가우시안 필터 적용한 이미지
	int* sobel_X = new int[img_size];//X gradient 
	int* sobel_Y = new int[img_size];//Y gradient
	unsigned char* Gradient = new unsigned char[img_size];//Gradient
	int* Angle = new int[img_size];//각 픽셀의 앵글-> 정규화
	int* TrueAngle = new int[img_size];//정규화되지 않은 기존 앵글
	unsigned char* Edge = new unsigned char[img_size];//엣지

	//테스트 이미지 load
	FILE* fp = fopen("Test_img_CV_HW4_396x400.yuv", "rb");
	fread(arr_img, sizeof(unsigned char), img_size, fp);
	fclose(fp);

	//가우시안 필터 적용을 위한 패딩
	padding_g(arr_img, Padded_img);

	//가우시안 필터 적용
	blurring(Padded_img, Blurred_img);
	FILE* wp = fopen("gaussian.yuv", "wb");
	fwrite(Blurred_img, sizeof(unsigned char), img_size, wp);
	fclose(wp);

	//소벨 필터를 위한 패딩
	padding_s(Blurred_img, Padded_img_s);

	//소벨 필터 사용하여 X gradient, Y gradient 얻음
	Getsobel(Padded_img_s, sobel_X, sobel_Y);

	//위에서 얻은 X, Y gradient로 각 픽셀의 Gradient, angle, 정규화되지 않은 true angle 얻음
	GetGradient_n_Angle(sobel_X, sobel_Y, Gradient, Angle, TrueAngle);
	wp = fopen("gradient.yuv", "wb");
	fwrite(Gradient, sizeof(unsigned char), img_size, wp);
	fclose(wp);

	//강한, 얇은 경계선 검출
	Non_maximum_suppression(Gradient, Angle, Edge);
	wp = fopen("edge.yuv", "wb");
	fwrite(Edge, sizeof(unsigned char), img_size, wp);
	fclose(wp);

	//경계선 추적, 잘못된 경계선 제거 및 최종 경계선 판단
	edge_tracking(Edge);
	wp = fopen("edgetracking.yuv", "wb");
	fwrite(Edge, sizeof(unsigned char), img_size, wp);
	fclose(wp);
	
	//허프 트랜스폼
	HoughTransform(Edge, Angle, TrueAngle);

}
