# CV_4
Hough Transform

Canny edge 검출 알고리즘과 Hough transform 알고리즘을 이용해

이미지 내 존재한는 다수의 원의 정보 검출

Canny edge 검출
- Sobel mask 적용하여 x, y축 gradient 추출
- x, y축의 gradient를 바탕으로 각 픽셀별 gradient와 edge의 각도 추출
- 주어진 정보를 기반으로 two-level thresholding 을 이용해 경계선 검출
