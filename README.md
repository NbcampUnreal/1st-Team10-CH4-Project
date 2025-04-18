# Arena Fighters
![1111](https://github.com/user-attachments/assets/7c987d78-9ea0-4276-9e50-eb9114825aa2)
Unreal의 Listen Server를 활용한 대전 격투 게임
## 🖥️ 개요
### 0. 목적
1) 객체지향과 디자인패턴을 의미있게 적용
2) 템플릿 기반의 프로젝트를 C++로 전환하여 개발
3) Unreal Engine 네트워크 프로그래밍 RPC, Replecates를 이용하여 Network 환경 조성
4) 멀티플레이 환경에서도 제대로 작동하기 위한 게임 최적화

### 1. 게임 모드 설명
1) 싱글 플레이
   - 커멘드 가이드 및 연습 스테이지
   - 일반 AI가 나오는 메인 스테이지 + 보스 AI와 전투 스테이지 구성
   - 낙사 조건 없이 HP가 0이 되면 플레이어 및 AI 사망 처리
   - 플레이어 사망 시, Restart로 다시 처음부터 시작
2) 협력 모드
   - 2인 이상 ~ 4인까지 플레이어와 협력하여 AI와 인원수 맞춰 대결
   - 시간 제한 안에 AI를 모두 쓰러트리면 승리
   - AI가 남아 있다면 패배
3) 대전 모드
   - 1 vs 1 혹은 2 vs 2로 팀원을 구성하여 게임을 진행
   - 시간 제한 안에 상대방을 모두 쓰러트리면 승리
   - 타임 아웃 시 서든데스 룰로 한 방 싸움 진행
### 2. 팀원
- 김동현
- 김성진
- 이학민
- 정완훈
- 김주영
- 유승완
### 3. 작업 기간
- 2025.03.31 ~ 2025.04.18
### 4. 기술 스택
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white)
![Rider](https://img.shields.io/badge/Rider-000000.svg?style=for-the-badge&logo=Rider&logoColor=white&color=black&labelColor=crimson)
![Unreal Engine](https://img.shields.io/badge/unrealengine-%23313131.svg?style=for-the-badge&logo=unrealengine&logoColor=white)
![Git](https://img.shields.io/badge/git-%23F05033.svg?style=for-the-badge&logo=git&logoColor=white)
![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)
![Blender](https://img.shields.io/badge/blender-%23F5792A.svg?style=for-the-badge&logo=blender&logoColor=white)

## 🎮 주요 구현
![2222](https://github.com/user-attachments/assets/0cf0953f-ed24-4e73-99fc-dae5035a1974)

<details>
<summary>1. GameMode</summary>
<div markdown="1">      

```
|-- Source
   |-- GameModes
      |-- CSGameModeBase
      |-- CSCoopGameMode
      |-- CSLobbyGameMode
      |-- CSMainMenuGameMode
      |-- CSSingleGameMode
      |-- CSVersusGameMode
```
</div>
</details>
<details>
<summary>2. Characters</summary>
<div markdown="1">      

```
|-- Source
   |-- Characters
      |-- Type
         |-- CSFighterCharacter
         |-- CSSwordManCharacter   
      |-- CSBaseCharacter
      |-- CSPlayerCharacter
   |-- Components
      |-- CSAttributeComponent
      |-- CSCombatComponent
```
</div>
</details>
<details>
<summary>3. AI</summary>
<div markdown="1">      

```
|-- Source
   |-- AI
      |-- Character
         |-- AIBaseCharacter
         |-- AIBossCharacter
         |-- AIFighterCharacter
         |-- AINormalCharacter
         |-- AISwordManCharacter
```
</div>
</details>
<details>
<summary>4. UI</summary>
<div markdown="1">      

```
|-- Source
   |-- UI
      |-- CSUIBaseWidget
      |-- CSLobbyBaseWidget
      |-- CSBossHud
      |-- CSCoopHud
      |-- CSCoopLobbyWidget
      |-- CSInGameHud
      |-- CSMainMenu
      |-- CSPlayerEntry
      |-- CSStageHud
      |-- CSVersusHud
      |-- CSVersusLobbyWidget
```
</div>
</details>
<details>
<summary>5. Map</summary>
<div markdown="1">      

```
|-- Content
   |-- Blueprints
      |-- Hud
         |-- Maps
            |-- MainMenuLevel
            |-- CoopModeLevel
            |-- LobbyLevel
            |-- SingleModeLevel
            |-- VersusModeLevel
```
</div>
</details>
