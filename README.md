# FeatureCatcher

  1. [프로젝트 설명](#1-프로젝트-설명)

  2. [개발 환경 구축 및 설치](#2-개발-환경-구축-및-설치)

  3. [특징](#3-특징)

  4. [사용법](#4-사용법)

# 1. 프로젝트 설명
FeatureCatcher는 OpenCV와 pre-trained 된 intel(c) OpenVINO의 모델과 Caffe의 모델을 사용하여 영상에서 사람과 해당 사람에 대한 특징들을 검출합니다. 특징에는 얼굴, 성별, 나이, 의상 색상이 있습니다. 검출된 특징들은 해당하는 사람 별로 DB를 구축합니다. 사용자는 웹을 통해 자신이 원하는 특징으로 영상 속에서 사람을 검색할 수 있습니다.

* [시연 영상](https://www.youtube.com/watch?v=geSkcmRTUK4)

# 2. 개발 환경 구축 및 설치
<strong>2-1 개발 환경</strong> 
  * 개발 OS는 Window 10 환경에서 진행되었습니다.


<strong>2-2 사전 설치 프로그램</strong>
  
  * 2-2-1 [OpenCV 4.4.0-OpenVINO 라이브러리 포함](https://github.com/opencv/opencv/releases/tag/4.4.0)
    * Debug용 : opencv-4.4.0-dldt-2020.4-vc16-avx2-debug.7z
    * Release용 : opencv-4.4.0-dldt-2020.4-vc16-avx2.7z

  * 2-2-2 [MySQL(8.0.21)](https://dev.mysql.com/downloads/installer/)
    * 자세한 설치 방법은 [이](https://dog-developers.tistory.com/20) 링크로 확인하시면 됩니다.

<strong>2-3 FeatureCatcher 설치</strong>
cmd창을 열고 설치하려는 경로로 이동합니다.
Git으로 부터 해당 프로젝트를 다운받습니다.
```
$ cd 설치 경로
$ git clone https://github.com/intaekimme/oss_dev_competition.git
```

# 3. 특징
해당 프로그램은 영상을 분석하여 영상에 출현한 사람들에 다양한 특징들을 뽑아내고 DB에 저장, 웹 UI를 통해 분석 내용을 확인하는 것을 목적으로 합니다.

- Feture Extraction을 위한 계층 구조
  먼저 영상 내의 개별 frame에서 pre-trained 된 모델로 사람을 감지합니다. 이 후 감지된 사람별로 ID 감지, 상하의 색상 감지, 얼굴 감지 모델을 적용 시킵니다. 얼굴 감지 모델의 경우에는 하위로 나이, 성별 감지 모델을 적용시킵니다. 

- DB 구성
  위에서 검출된 데이터를 가지고 구축된 DB에 데이터를 입력해줍니다.

- 웹 UI
  웹 UI는 아직 미완성입니다. 

<img src="/document/feature.jpg" width="100%" height="100%" title="특징" alt="Feature"></img>

# [4. 사용법](/document/How_to_Use.md)
