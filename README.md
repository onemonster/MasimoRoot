# IAP Client (Masimo Root)

## 프로세스

- Keep Alive Process: 2초 마다 keep alive command를 날려줍니다.
- Read Process: 응답을 파싱해서 출력해줍니다.
- Main Process: 원하는 command를 날리는 process입니다.
