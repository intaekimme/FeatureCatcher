# FeatureCatcher

  1. 프로젝트 설명(링크 처리)

  2. 개발 환경 구축 및 설치(링크 처리)

  3. 특징(링크 처리)

  4. 사용법(링크 처리)

# 1. 프로젝트 설명
FeatureCatcher는 OpenCV와 pre-trained 된 intel(c) OpenVINO의 모델과 Caffe의 모델을 사용하여 영상에서 사람과 해당 사람에 대한 특징들을 검출합니다. 특징에는 얼굴, 성별, 나이, 의상 색상이 있습니다. 검출된 특징들은 해당하는 사람 별로 DB를 구축합니다. 사용자는 웹을 통해 자신이 원하는 특징으로 영상 속에서 사람을 검색할 수 있습니다.

# 2. 개발 환경 구축 및 설치
2-1 개발 환경(굵은 글씨)
  * 개발 OS는 Window 10 환경에서 진행되었습니다.


2-2 사전 설치 프로그램(굵은 글씨)
  
  2-2-1. [OpenCV 4.4.0](https://github.com/intaekimme/oss_dev_competition/blob/master/document/OpenCV.md)
  
  2-2-2. [MySQL 8.0.21](https://github.com/intaekimme/oss_dev_competition/blob/master/document/MySQL.md)
  
  2-2-3. etc...(추가적인것 있으면 넣어주고 링크로 처리)

2-3 FeatureCatcher 설치(굵은 글씨)
cmd창을 열고 설치하려는 경로로 이동합니다.
Git으로 부터 해당 프로젝트를 다운받습니다.
```
$ cd 설치 경로
$ git clone https://github.com/intaekimme/oss_dev_competition.git
```

# 3. 특징

# 4. 사용법
분석을 원하는 영상을 FeatureCatcher 경로 안에 넣어줍니다.
이후 FeatureCatcher.exe를 실행