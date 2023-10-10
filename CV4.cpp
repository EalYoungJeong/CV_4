#include<iostream>
#include<fstream>
#include<cmath>

using namespace std;
#define WIDTH 396	//���α���
#define HEIGHT 400	//���α���
#define img_size WIDTH*HEIGHT	//�̹��� ũ��
#define HIGH_th 150	// high threshold
#define LOW_th 50	//low threshold

void padding_g(unsigned char* src, unsigned char* dest)
{//����þ� ����(5x5) ������ ���� ���� �̹��� same padding
	unsigned char first_c;
	unsigned char last_c;
	int padded_idx = 0;
	//���� 2�� same padding
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

	//�߰� 400�� same padding
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

	//���� 2�� same padding
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
{//�Һ�  ����ũ(3x3) ������ ���� ���� �̹��� same padding
	unsigned char first_c;
	unsigned char last_c;
	int padded_idx = 0;
	//���� 1�� same padding
	dest[padded_idx++] = src[0];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[i];
	dest[padded_idx++] = src[WIDTH - 1];

	//�߰� 400�� same padding
	for (int m = 0; m < HEIGHT; m++)
	{
		first_c = src[m * WIDTH];
		dest[padded_idx++] = first_c;
		for (int i = 0; i < WIDTH; i++)
			dest[padded_idx++] = src[m * WIDTH + i];
		last_c = src[(m + 1) * WIDTH - 1];
		dest[padded_idx++] = last_c;
	}

	//���� 1�� same padding
	dest[padded_idx++] = src[WIDTH * (HEIGHT - 1)];
	for (int i = 0; i < WIDTH; i++)
		dest[padded_idx++] = src[WIDTH * (HEIGHT - 1) + i];
	dest[padded_idx++] = src[WIDTH * HEIGHT - 1];
}
void blurring(unsigned char* src, unsigned char* dest)//����þ� ���� ����
{
	//����þ� ����ũ
	double GaussianMask[5][5] = { {2,4,5,4,2},
								{4,9,12,9,4},
								{5,12,15,12,5},
								{4,9,12,9,4},
								{2,4,5,4,2} };
	int temp_sum = 0;

	//�� �ȼ� �� convolution ����
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
			dest[i * WIDTH + j] = temp_sum / 159;//�� ����ȭ �� �ȼ� �迭�� ����
		}
	}
}
void Getsobel(unsigned char* src, int* sobelX, int* sobelY)//�Һ� ���� ���� �� X, Y Gradient ����
{
	int Sobel_x[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1} };//X �Һ� ����ũ
	int Sobel_y[3][3] = { {1,2,1},{0,0,0},{-1,-2,-1} };//Y �Һ� ����ũ
	int temp_sum = 0;

	//convolution ����
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
void GetGradient_n_Angle(int* gradient_X, int* gradient_Y, unsigned char* gradient, int* angle, int* trueangle)//Gradient, angle, True angle ����
{
	double temp_angle;
	const double PI = 3.141592;//����
	for (int i = 0; i < img_size; i++)
	{
		gradient[i] = sqrt((gradient_X[i] * gradient_X[i]) + (gradient_Y[i] * gradient_Y[i]));//��Ÿ��� ������ ���� Gradient ũ��

		temp_angle = atan2(gradient_X[i], gradient_Y[i]) / PI * 180;//arctan �Լ��� X�� Y �� ���� ���� theta ���ϱ�, 180/PI => ���� ��ȭ
		trueangle[i] = temp_angle;//����ȭ ���� ���� ���� ����

		//���� ����ȭ 0, 45, 90, 135 ����
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
		angle[i] = temp_angle;//����ȭ�� ���� ����
	}
}
void Non_maximum_suppression(unsigned char* gradient, int* angle, unsigned char* edge)//��輱 ����
{
	bool isMax;//�ֺ� �ȼ� �� ���� ū�� Ȯ���ϴ� ����
	memset(edge, 0, sizeof(unsigned char) * img_size);

	for (int i = 1; i < HEIGHT - 1; i++)
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			int cur_idx = (i * WIDTH) + j;//1���� -> 2���� ����
			isMax = false;
			switch (angle[cur_idx])//����ȭ ������ ����
			{//�ش� ������ ���� ���� �ȼ��� ���Ͽ� �ڽ��� ���� ū ���� ��� true ��ȯ
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

			if (isMax)//���� true�ϰ��
			{
				if (gradient[cur_idx] > HIGH_th)//���� ��輱
					edge[cur_idx] = 255;
				else if (gradient[cur_idx] > LOW_th)//���� ��輱
					edge[cur_idx] = 128;
				else//����
					edge[cur_idx] = 0;
			}
		}
	}
}

void edge_tracking(unsigned char* src)
{
	for (int i = 1; i < HEIGHT - 1; i++)//��ü �ȼ� Ž��
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			int cur_idx = (i * WIDTH) + j;
			if (src[cur_idx] == 255)//���� ��輱 ���ؿ��� ������ ���� ��輱�� ���� ���
			{//���� ��輱 ���� ���� ��輱���� ����
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

	//���� ��輱�� ���� ���� ���� ��輱 ��� ����
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
	int* Acc = new int[img_size];//��ǥ�� ���� ���� �����
	for (int i = 0; i < img_size; i++)//�ʱ�ȭ
		Acc[i] = 0;
	int MaxR = 100;//�ִ� ������ 100

	for (int i = 1; i < HEIGHT - 1; i++)
	{
		for (int j = 1; j < WIDTH - 1; j++)
		{
			int cur_idx = (i * WIDTH) + j;//1���� -> 2���� ����
			if (edge[cur_idx] == 255)//���� ���� �� �ȼ� ����
			{
				for (int r = 10; r < MaxR; r++)//�ȼ� ��ǥ (i,j) ���� ������ ���� ����� ��
				{
					int th = angle[cur_idx];//���� �ȼ��� angle ����
					//���� �ȼ��� angle ������ True angle�� ���� ������ ����� ������ �������� ��ǥ
					if (th == 0 && (abs(TrueAngle[cur_idx]-th)<=3 || abs(TrueAngle[cur_idx]-th)>=177))//theta = 0 �϶� �����ϴ� �������� ��ǥ
					{
						if (cur_idx + (r * WIDTH) < img_size && cur_idx + (r * WIDTH) >= 0)
							Acc[cur_idx + (r * WIDTH)]++;
						if (cur_idx - (r * WIDTH) < img_size && cur_idx - (r * WIDTH) >= 0)
							Acc[cur_idx - (r * WIDTH)]++;
					}
					if (th == 45 && abs(TrueAngle[cur_idx]-th)<=3)//theta = 45 �϶� �����ϴ� 135�� ���� ��ǥ
					{
						if (cur_idx + (r * WIDTH) - r >= 0 && cur_idx + (r * WIDTH) - r < img_size)
							Acc[cur_idx + (r * WIDTH) - r]++;
						if (cur_idx - (r * WIDTH) + r >= 0 && cur_idx - (r * WIDTH) + r < img_size)
							Acc[cur_idx - (r * WIDTH) + r]++;
					}
					if (th == 90 && abs(TrueAngle[cur_idx] - th) <= 3)//theta = 90 �϶� �����ϴ� ������� ��ǥ
					{
						if (cur_idx + r < img_size && cur_idx + r >= 0)
							Acc[cur_idx + r]++;
						if (cur_idx - r < img_size && cur_idx - r >= 0)
							Acc[cur_idx - r]++;
					}
					if (th == 135 && abs(TrueAngle[cur_idx] - th) <= 3)//theta = 135 �϶� �����ϴ� 45�� ���� ��ǥ
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

	//������ Ȯ�ο� �׽�Ʈ �̹��� ���
	FILE* wp = fopen("test.yuv", "wb");
	unsigned char* test = new unsigned char[img_size];
	int Ranking[4][3] = {0};
	
	//�� �κ�(�»�, ���, ����, ����)�� �ִ� ��ǥ���� ���� �ȼ� ���� 
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			int cur_idx = (i * WIDTH) + j;
			if (Acc[cur_idx] > 0)//��ǥ ������� ���� �ȼ� ��
			{
				//cout << Acc[cur_idx] << " " << i << " " << j << endl;//�ĺ� �ȼ� ���
				test[cur_idx] = 255;//�̹����� ���

				//��� �ȼ� Ž���ϸ� ��Ʈ �� �ִ밪 �ֽ�ȭ
				if (i < 200)
				{
					if (j < 196)//�»�
					{
						if (Ranking[0][2] <= Acc[cur_idx])
						{
							Ranking[0][2] = Acc[cur_idx];
							Ranking[0][0] = i;
							Ranking[0][1] = j;
						}
					}
					else//���
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
					if (j < 196)//����
					{
						if (Ranking[2][2] <= Acc[cur_idx])
						{
							Ranking[2][2] = Acc[cur_idx];
							Ranking[2][0] = i;
							Ranking[2][1] = j;
						}
					}
					else//����
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
	cout << "�� ������ �ִ� ��ǥ ��ǥ�� ��ǥ��" << endl;
	for (int i = 0; i < 4; i++)//�� �κ� �� �ִ� ��ǥ ��ǥ ���
	{
		cout << i + 1 <<". " << Ranking[i][0] << " " << Ranking[i][1] << "    " << Ranking[i][2] << endl;
	}

	cout << endl << endl;
	///���� ��ǥ�� �������� ������ ���ϱ�
	for (int i = 0; i < 4; i++)
	{
		int cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//�ִ� ��ǥ ��ǥ�� 1���� ����
		int dist = 0;//������ 
		int dist_av = 0;//������ ���
		int dist_c = 0;//������ ����
		
		//���� ��ǥ�� �������� 8���⿡ ���� ���� ���� �� �������� �Ÿ� dist
		while (1)
		{//����
			if (edge[cur_idx--] == 255) break;
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

		
		while (1)
		{//������
			if (edge[cur_idx++] == 255) break;
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

		
		while (1)
		{//�Ʒ���
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx + WIDTH;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

		
		while (1)
		{//����
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx + WIDTH;
				cur_idx = cur_idx + 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

		
		while (1)
		{//�»�
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx - WIDTH;
				cur_idx = cur_idx - 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

		
		while (1)
		{//���
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx - WIDTH;
				cur_idx = cur_idx + 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

	
		while (1)
		{//����
			if (edge[cur_idx] == 255) {
				cur_idx = cur_idx + WIDTH;
				cur_idx = cur_idx - 1;
				break;
			}
			else dist++;
			if (dist > MaxR) break;
		}
		if (dist < 100)//������ ã�� ���
		{
			dist_av = dist_av + dist;//���� ����
			dist_c++;//���� ����
		}
		dist = 0;//�Ÿ� �ʱ�ȭ
		cur_idx = Ranking[i][0] * WIDTH + Ranking[i][1];//��ǥ �ʱ�ȭ

	
		while (1)
		{//����
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
		if (dist_c == 0)//������ �ϳ��� ���� ���� ��� ����
		{
			cout << "����" << endl;
			continue;
		}
		else//������ ��� ��� ����
			dist_av = dist_av / dist_c;

		cout << "a , b =  " << Ranking[i][1] << " , " << Ranking[i][0] << "    ������ : " << dist_av << endl; //������
	}
}

int main()
{
	unsigned char* arr_img = new unsigned char[img_size];//�׽�Ʈ �̹���
	unsigned char* Padded_img = new unsigned char[(WIDTH + 4) * (HEIGHT + 4)];//����þ� ���͸� ���� �е�
	unsigned char* Padded_img_s = new unsigned char[(WIDTH + 2) * (HEIGHT + 2)];//sobel ����ũ ������ ���� �е�
	unsigned char* Blurred_img = new unsigned char[img_size];//����þ� ���� ������ �̹���
	int* sobel_X = new int[img_size];//X gradient 
	int* sobel_Y = new int[img_size];//Y gradient
	unsigned char* Gradient = new unsigned char[img_size];//Gradient
	int* Angle = new int[img_size];//�� �ȼ��� �ޱ�-> ����ȭ
	int* TrueAngle = new int[img_size];//����ȭ���� ���� ���� �ޱ�
	unsigned char* Edge = new unsigned char[img_size];//����

	//�׽�Ʈ �̹��� load
	FILE* fp = fopen("Test_img_CV_HW4_396x400.yuv", "rb");
	fread(arr_img, sizeof(unsigned char), img_size, fp);
	fclose(fp);

	//����þ� ���� ������ ���� �е�
	padding_g(arr_img, Padded_img);

	//����þ� ���� ����
	blurring(Padded_img, Blurred_img);
	FILE* wp = fopen("gaussian.yuv", "wb");
	fwrite(Blurred_img, sizeof(unsigned char), img_size, wp);
	fclose(wp);

	//�Һ� ���͸� ���� �е�
	padding_s(Blurred_img, Padded_img_s);

	//�Һ� ���� ����Ͽ� X gradient, Y gradient ����
	Getsobel(Padded_img_s, sobel_X, sobel_Y);

	//������ ���� X, Y gradient�� �� �ȼ��� Gradient, angle, ����ȭ���� ���� true angle ����
	GetGradient_n_Angle(sobel_X, sobel_Y, Gradient, Angle, TrueAngle);
	wp = fopen("gradient.yuv", "wb");
	fwrite(Gradient, sizeof(unsigned char), img_size, wp);
	fclose(wp);

	//����, ���� ��輱 ����
	Non_maximum_suppression(Gradient, Angle, Edge);
	wp = fopen("edge.yuv", "wb");
	fwrite(Edge, sizeof(unsigned char), img_size, wp);
	fclose(wp);

	//��輱 ����, �߸��� ��輱 ���� �� ���� ��輱 �Ǵ�
	edge_tracking(Edge);
	wp = fopen("edgetracking.yuv", "wb");
	fwrite(Edge, sizeof(unsigned char), img_size, wp);
	fclose(wp);
	
	//���� Ʈ������
	HoughTransform(Edge, Angle, TrueAngle);

}
