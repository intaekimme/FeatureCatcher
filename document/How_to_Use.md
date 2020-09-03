# 사용법
<strong>FeatureCatcher을 정상적으로 사용하기 위해선 아래 항목을 모두 수행해야 합니다.</strong>

## FeatureCatcher 사용법

새로운 영상에 관하여 영상 분석을 원할 경우 다음과 같이 합니다.
<img src="/document/use.png" width="100%" height="100%" title="객체설정" alt="Use"></img>

FeatureCatcher 프로그램은 main.cpp 에서 FeatureCatcher 객체를 통해 이루어집니다. FeatureCatcher 객체 선언 예시는 다음과 같습니다.
``` c++
FeatureCatcher FC("video_test2.mp4", 1, "videoLog.txt", "C:/Bitnami/wampstack-7.4.8-0/apache2/htdocs/0903_2",
		"localhost", 3306, "root", "1111", "video_detector", "table_videoanalyzing", 10, true);
```

각 인수는 순서대로 다음과 같습니다.
* 비디오 이름
* 몇 프레임마다 분석을 진행할 것인지 설정
* 로그 저장 파일 이름 (확장자는 txt로 설정)
* 웹 서버 경로 <strong>(예를 들어 Apache라면, htdocs 폴더 경로로 설정. 또한 그곳에 person_img 폴더를 만들어 주어야 이미지가 저장된다.)</strong>
    > 저희는 웹 어플리케이션을 위해 Apache, php, MySQL 을 사용하였습니다. 이것을 통합 관리해주는 Bitnami 프로그램을 사용합니다.
        > [Bitnami-설치법](https://capakhy64.tistory.com/3) 
* MySQL 서버 IP
* MySQL 사용자 이름
* MySQL 서버 PassWord
* 해당 데이터베이스 이름 (default : video_detector)
* MySQL 서버 Port
* 해당 테이블 이름 (default : table_videoanalyzing)
* 몇 프레임마다 이미지, 로그 저장을 할 것인지
* 영상에 분석 데이터 시각화를 해줄 것인지


## MySQL 초기 설정
* 데이터베이스 생성
``` sql
CREATE DATABASE video_detector;
```

* 테이블 생성
```sql
CREATE TABLE `table_videoanalyzing` (
   `id` int(11) DEFAULT NULL,
   `frame` int(11) DEFAULT NULL,
   `millisec` int(11) DEFAULT NULL,
   `age` int(11) DEFAULT NULL,
   `gender` varchar(10) DEFAULT NULL,
   `img_person` varchar(100) DEFAULT NULL,
   `top_color` int(11) DEFAULT NULL,
   `bottom_color` int(11) DEFAULT NULL
 ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
 ```

## php-code 사용법(미완성)
* MySQL 연결
    > index2.php, index3.php - 77줄 - mysql 정보 입력
``` php
$conn = mysqli_connect("127.0.0.1", "root", "1111", "video_detector", 3306);
```

* 미완성이라 웹에 분석한 영상을 표시할 때, 나타나는 영상을 바꾸려면 해당 코드 부분을 수정한다.
    >index.php - 42줄 
    ``` php
    <video src="./media/video_test2.mp4" autoplay controls width="1000px" height="500"></video>
    ```
    > index2.php, index3.php - 140줄
    ``` php
    <source src="./media/video_test2.mp4" type="video/mp4" />
    ```

# 프로그램 재사용시 주의
<strong>정상적인 프로그램 사용을 위해서는 다음을 수행해야 합니다.</strong>

* 이미지 로그 제거
웹 서버  경로 내 person_img 폴더 아래에 있는 이미지 파일들을 모두 제거합니다.

* 로그 저장 파일 초기화
exe 파일이 있는 곳의 videoLog.txt의 내용을 모두 지우고 저장합니다.

* DB 테이블 초기화
``` sql
DELETE FROM video_detector.table_videoanalyzing;
```
