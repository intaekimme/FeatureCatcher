# FeatureCatcher

  1. [프로젝트 설명](#1-프로젝트-설명)
  
  2. [특징](#2-특징)
  
  3. [아키텍쳐](#3-아키텍쳐)

  4. [개발 환경 구축 및 설치](#4-개발-환경-구축-및-설치)  

  5. [사용법](#5-사용법)


# 1. 프로젝트 설명
FeatureCatcher는 OpenCV와 pre-trained 된 intel(c) OpenVINO의 모델과 Caffe의 모델을 사용하여 영상에서 사람과 해당 사람에 대한 특징들을 검출합니다. 특징에는 얼굴, 성별, 나이, 의상 색상이 있습니다. 검출된 특징들은 해당하는 사람 별로 DB를 구축합니다. 사용자는 웹을 통해 자신이 원하는 특징으로 영상 속에서 사람을 검색할 수 있습니다.

* [시연 영상](https://www.youtube.com/watch?v=geSkcmRTUK4)



# 2. 특징
해당 프로그램은 영상을 분석하여 영상에 출현한 사람들에 다양한 특징들을 뽑아내고 DB에 저장, 웹 UI를 통해 분석 내용을 확인하는 것을 목적으로 합니다.

- Feture Extraction을 위한 계층 구조
  먼저 영상 내의 개별 frame에서 pre-trained 된 모델로 사람을 감지합니다. 이 후 감지된 사람별로 ID 감지, 상하의 색상 감지, 얼굴 감지 모델을 적용 시킵니다. 얼굴 감지 모델의 경우에는 하위로 나이, 성별 감지 모델을 적용시킵니다. 

- DB 구성
  위에서 검출된 데이터를 가지고 구축된 DB에 데이터를 입력해줍니다.

- 웹 UI
  분석 완료된 영상 항목들을 리스트업합니다. 영상 이름을 클릭하면 해당 영상에서 분석된 사람들을 ID별로 묶어서 데이터를 시각화합니다.

<img src="/document/feature.jpg" width="100%" height="100%" title="특징" alt="Feature"></img>



# 3. 아키텍쳐

<img src="/document/architecture_featurecatcher.png" height="100%" title="아키텍쳐" alt="architecuture_featurecatcher"></img>



# 4. 개발 환경 구축 및 설치
<strong>2-1 개발 환경</strong> 
  * OS : Ubuntu 20.04.1 LTS focal, x86_64



<strong>2-2 사전 설치 프로그램</strong>
  
  * 2-2-1 [OpenCV 4.4.0-OpenVINO 라이브러리 포함](https://github.com/opencv/opencv/releases/tag/4.4.0)
    * main module : OpenCV 4.4.0
      download path :
       https://github.com/opencv/opencv/tags
       
    * extra module : opencv_contrib 4.4.0
      download path : 
      https://github.com/opencv/opencv_contrib/releases
      
    * OpenCV 컴파일 전 필요 패키지
      - build-essential
      - cmake
      - pkg-config
      - libjpeg-dev libjpeg-dev libpng-dev
      - ffmpeg libavcodec-dev libavformat-dev libswscale-dev
       libxvidcore-dev libx264-dev libxine2-dev
      - libv4l-dev v4l-utils
      - libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev 
      - libgtk-3-dev
      - mesa-utils libgl1-mesa-dri libgtkgl2.0-dev libgtkglext1-dev
      - libatlas-base-dev gfortran libeigen3-dev
      
  * 2-2-2 [OpenVINO : (install guide path)] 
        - intel-openvino-dev-ubuntu18-2020.4.287
        - (이 프로젝트에서 사용한 버전) 
        https://docs.openvinotoolkit.org/2020.4/openvino_docs_install_guides_installing_openvino_apt.html


  * 2-2-3 [MySQL : Ver 8.0.21-0ubuntu0.20.04.4 for Linux on x86_64 ((Ubuntu))]
      - mysql.h 사용하기위해
      - $ apt-get install libmysqlclient-dev
      
      [ mysql.h 위치 찾기 ]
      mysql_config –cflags

      [ include  mysql.h 방법 ]
      #include "/usr/include/mysql/mysql.h" (위에서 찾은 위치경로)
      
      [ 컴파일 방법 ]
      g++ -o 프로그램명 파일명.c -lmysqlclient

      "/usr/lib/x86_64-linux-gnu/libmysqlclient.so"

      - mysql 옵션 변경
      - set root user to mysql_native_password

      - $ sudo mysql -u root -p # I had to use "sudo" since is new installation

      - mysql:~ USE mysql;
      - SELECT User, Host, plugin FROM mysql.user;
      - mysql:~ UPDATE user SET plugin='mysql_native_password' WHERE User='root';
      - mysql:~ FLUSH PRIVILEGES;
      - mysql:~ exit;

      $ service mysql restart
      
    * 최초에 featurecatcher
      데이터베이스 생성 
      CREATE DATABASE featurecatcher;


<strong>2-3 FeatureCatcher 설치</strong>
cmd창을 열고 설치하려는 경로로 이동합니다.
Git으로 부터 해당 프로젝트를 다운받습니다.
```
$ cd 설치 경로
$ git clone https://github.com/intaekimme/oss_dev_competition.git
```



# [5. 사용법](/document/How_to_Use.md)
