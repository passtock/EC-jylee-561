import cv2

print("📷 연결된 모든 카메라를 검색합니다...")

# 0번부터 9번까지 다 찔러보기
for index in range(10):
    cap = cv2.VideoCapture(index, cv2.CAP_DSHOW) # 윈도우에서는 CAP_DSHOW가 정확함
    
    if cap.isOpened():
        ret, frame = cap.read()
        if ret:
            print(f"✅ 카메라 발견! 번호: {index}")
            print(f"   - 해상도: {int(cap.get(3))} x {int(cap.get(4))}")
            # 이 카메라가 DroidCam인지 확인하기 위해 창을 띄워봄
            cv2.imshow(f'Camera {index}', frame)
            cv2.waitKey(1000) # 1초 보여줌
            cv2.destroyAllWindows()
        else:
            print(f"⚠️ 번호 {index}: 열리긴 했는데 화면이 안 나옵니다.")
    else:
        print(f"❌ 번호 {index}: 장치 없음")
    
    cap.release()

print("검색 종료. 위에서 화면이 뜬 번호를 movenet.py에 넣으세요.")