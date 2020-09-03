# FeatureCatcher 사용법

## 1. 시스템 환경 변수 편집
시스템 환경 변수 편집은 다음 사진 순서대로 해줍니다.

1. 
<img src="/document/sys_setting_1.png" width="100%" height="100%" title="특징" alt="Feature"></img>

2. 
<img src="/document/sys_setting_2.png" width="100%" height="100%" title="특징" alt="Feature"></img>

3. 
<img src="/document/sys_setting_3.png" width="100%" height="100%" title="특징" alt="Feature"></img>

3번 단계에서는 아래와 같은 환경 변수를 추가해줍니다.
```
C:\Program Files\MySQL\MySQL Shell 8.0\bin

C:\"OpenCV 4.4.0-OpenVINO 라이브러리 포함 설치경로"\opencv\build\bin
```

주의)
* "OpenCV 4.4.0-OpenVINO 라이브러리 포함 설치경로" 를 추가할 때는 Debug 와 Release를 각 각 추가해야합니다.

* " " 경로는 개인이 지정한 경로로 개인마다 다를 수 있습니다.

## 2. 프로젝트 속성 설정(Release)
프로젝트 환경 변수 설정은 다음 사진 순서대로 해줍니다.

1. 프로젝트(P) - 속성(P) 를 눌러줍니다.
<img src="/document/proj_setting_1.png" width="100%" height="100%" title="특징" alt="Feature"></img>

2. 구성(C) 에서 Release, 플랫폼(P) 에서 x64 선택 후 C/C++ - 일반 항목으로 이동해줍니다.
<img src="/document/proj_setting_2.png" width="100%" height="100%" title="특징" alt="Feature"></img>

3. 추가 포함 디렉터리 - <편집> 을 선택합니다.
<img src="/document/proj_setting_3.png" width="100%" height="100%" title="특징" alt="Feature"></img>

4. 줄 추가를 선택 후 다음 경로를 입력해 줍니다.
```
C:\Program Files\MySQL\MySQL Server 8.0\include

C:\Program Files\MySQL\Connector C++ 8.0\include

C:\"OpenCV 4.4.0-OpenVINO 라이브러리 포함 Release 버전 설치경로"\opencv\build\include
```
주의) 
* " " 경로는 개인이 지정한 경로로 개인마다 다를 수 있습니다.

<img src="/document/proj_setting_4.png" width="100%" height="100%" title="특징" alt="Feature"></img>

5. 링커 - 일반 항목으로 이동합니다.
<img src="/document/proj_setting_5.png" width="100%" height="100%" title="특징" alt="Feature"></img>

6. 추가 라이브러리 디렉터리 - <편집> 을 선택합니다.
<img src="/document/proj_setting_6.png" width="100%" height="100%" title="특징" alt="Feature"></img>

7. 줄 추가를 선택 후 다음 경로를 입력해 줍니다.
```
C:\Program Files\MySQL\MySQL Server 8.0\lib

C:\Program Files\MySQL\Connector C++ 8.0\lib64\vs14

C:\(OpenCV 4.4.0-OpenVINO 라이브러리 포함 Release 버전 설치경로)\opencv\build\lib
```
주의) 
* " " 경로는 개인이 지정한 경로로 개인마다 다를 수 있습니다.

<img src="/document/proj_setting_7.png" width="100%" height="100%" title="특징" alt="Feature"></img>

8. 링커 - 입력 항목으로 이동합니다.
<img src="/document/proj_setting_8.png" width="100%" height="100%" title="특징" alt="Feature"></img>

9. 다음 항목들을 입력해줍니다.
```
libmysql.lib
mysqlcppconn.lib
opencv_calib3d440.lib
opencv_core440.lib
opencv_dnn440.lib
opencv_features2d440.lib
opencv_flann440.lib
opencv_gapi440.lib
opencv_highgui440.lib
opencv_imgcodecs440.lib
opencv_imgproc440.lib
opencv_ml440.lib
opencv_objdetect440.lib
opencv_photo440.lib
opencv_stitching440.lib
opencv_video440.lib
opencv_videoio440.lib
```
<img src="/document/proj_setting_9.png" width="100%" height="100%" title="특징" alt="Feature"></img>

## 3. 프로젝트 속성 설정(Debug)
Debug 환경의 경우 Release와 설정 방법이 유사합니다. 다른 항목의 번호는 다음과 같습니다.

* 2번) Release 에서 Debug로 설정 해줍니다.

* 4번) C:\"OpenCV 4.4.0-OpenVINO 라이브러리 포함 Debug 버전 설치경로"\opencv\build\include 로 설정해줍니다. 

* 7번) C:\(OpenCV 4.4.0-OpenVINO 라이브러리 포함 Debug 버전 설치경로)\opencv\build\lib 로 설정해줍니다.

* 9번) OpenCV 관련 lib만 아래로 입력합니다.
```
opencv_calib3d440d.lib
opencv_core440d.lib
opencv_dnn440d.lib
opencv_features2d440d.lib
opencv_flann440d.lib
opencv_gapi440d.lib
opencv_highgui440d.lib
opencv_imgcodecs440d.lib
opencv_imgproc440d.lib
opencv_ml440d.lib
opencv_objdetect440d.lib
opencv_photo440d.lib
opencv_stitching440d.lib
opencv_video440d.lib
opencv_videoio440d.lib
```