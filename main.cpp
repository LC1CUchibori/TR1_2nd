#include <Novice.h>
#include <corecrt_math.h>

const char kWindowTitle[] = "LC1C_ウチボリ_ユウタ_タイトル";

struct Vector2
{
	float x;
	float y;
};

struct Player
{
	Vector2 position;
	float radius;
	Vector2 velocity;
	Vector2 dragStart;
	bool isDragging; 
};

struct Enemy
{
	Vector2 position;
	float radius;
	unsigned int color;
	int hitTimer;     
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 480, 720);

	// 画面のサイズ
	const int kScreenWidth = 480;
	const int kScreenHeight = 720;


	Player PenetrationPosition{ {100,650},25,{0,0},{0,0} ,false };

	Player ReflectionPosition{ {200,650},25,{0,0} ,{0,0},false };

	Enemy EnemyPosition{ { 240,360 },40 ,WHITE, 0 };

	int mouseX = 0;
	int mouseY = 0;
	int prevMouseX = 0;
	int prevMouseY = 0;
	

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//// プレイヤーがドラッグされているかどうかのフラグ
	//bool isPenetrationDragging = false;
	//bool isReflectionDragging = false;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// マウスの位置を取得する
		prevMouseX = mouseX;
		prevMouseY = mouseY;
		Novice::GetMousePosition(&mouseX, &mouseY);


		///
		/// ↓更新処理ここから
		///

		Novice::IsTriggerMouse(0);

		// マウスの左ボタンが押された瞬間
		if (Novice::IsTriggerMouse(0))
		{
			float dx1 = mouseX - PenetrationPosition.position.x;
			float dy1 = mouseY - PenetrationPosition.position.y;
			if ((dx1 * dx1 + dy1 * dy1) <= (PenetrationPosition.radius * PenetrationPosition.radius))
			{
				PenetrationPosition.dragStart.x = (float)mouseX;
				PenetrationPosition.dragStart.y = (float)mouseY;
				PenetrationPosition.isDragging = true;
			}

			float dx2 = mouseX - ReflectionPosition.position.x;
			float dy2 = mouseY - ReflectionPosition.position.y;
			if ((dx2 * dx2 + dy2 * dy2) <= (ReflectionPosition.radius * ReflectionPosition.radius))
			{
				ReflectionPosition.dragStart.x = (float)mouseX;
				ReflectionPosition.dragStart.y = (float)mouseY;
				ReflectionPosition.isDragging = true;
			}
		}

		// マウスの左ボタンが離された瞬間でドラッグ終了
		if (Novice::IsTriggerMouse(1))
		{
			if (PenetrationPosition.isDragging)
			{
				float dragDistanceX = mouseX - PenetrationPosition.dragStart.x;
				float dragDistanceY = mouseY - PenetrationPosition.dragStart.y;
				PenetrationPosition.velocity.x = -0.1f * dragDistanceX; 
				PenetrationPosition.velocity.y = -0.1f * dragDistanceY;
				PenetrationPosition.isDragging = false;
			}
			else if (ReflectionPosition.isDragging)
			{
				float dragDistanceX = mouseX - ReflectionPosition.dragStart.x;
				float dragDistanceY = mouseY - ReflectionPosition.dragStart.y;
				ReflectionPosition.velocity.x = -0.1f * dragDistanceX; 
				ReflectionPosition.velocity.y = -0.1f * dragDistanceY;
				ReflectionPosition.isDragging = false;
			}
		}

		// プレイヤーの位置を更新
		PenetrationPosition.position.x += PenetrationPosition.velocity.x;
		PenetrationPosition.position.y += PenetrationPosition.velocity.y;

		ReflectionPosition.position.x += ReflectionPosition.velocity.x;
		ReflectionPosition.position.y += ReflectionPosition.velocity.y;

		// 速度を減速する
		PenetrationPosition.velocity.x *= 0.98f; 
		PenetrationPosition.velocity.y *= 0.98f;

		ReflectionPosition.velocity.x *= 0.98f; 
		ReflectionPosition.velocity.y *= 0.98f;

		// 画面の境界を超えないように制限
		// 貫通
		if (PenetrationPosition.position.x - PenetrationPosition.radius < 0)
		{
			PenetrationPosition.position.x = PenetrationPosition.radius;
			PenetrationPosition.velocity.x = -PenetrationPosition.velocity.x;
		}
		else if (PenetrationPosition.position.x + PenetrationPosition.radius > kScreenWidth)
		{
			PenetrationPosition.position.x = kScreenWidth - PenetrationPosition.radius;
			PenetrationPosition.velocity.x = -PenetrationPosition.velocity.x;
		}

		if (PenetrationPosition.position.y - PenetrationPosition.radius < 0)
		{
			PenetrationPosition.position.y = PenetrationPosition.radius;
			PenetrationPosition.velocity.y = -PenetrationPosition.velocity.y;
		}
		else if (PenetrationPosition.position.y + PenetrationPosition.radius > kScreenHeight)
		{
			PenetrationPosition.position.y = kScreenHeight - PenetrationPosition.radius;
			PenetrationPosition.velocity.y = -PenetrationPosition.velocity.y;
		}

		// 反射
		if (ReflectionPosition.position.x - ReflectionPosition.radius < 0)
		{
			ReflectionPosition.position.x = ReflectionPosition.radius;
			ReflectionPosition.velocity.x = -ReflectionPosition.velocity.x;
		}
		else if (ReflectionPosition.position.x + ReflectionPosition.radius > kScreenWidth)
		{
			ReflectionPosition.position.x = kScreenWidth - ReflectionPosition.radius;
			ReflectionPosition.velocity.x = -ReflectionPosition.velocity.x;
		}

		if (ReflectionPosition.position.y - ReflectionPosition.radius < 0)
		{
			ReflectionPosition.position.y = ReflectionPosition.radius;
			ReflectionPosition.velocity.y = -ReflectionPosition.velocity.y;
		}
		else if (ReflectionPosition.position.y + ReflectionPosition.radius > kScreenHeight)
		{
			ReflectionPosition.position.y = kScreenHeight - ReflectionPosition.radius;
			ReflectionPosition.velocity.y = -ReflectionPosition.velocity.y;
		}

		// 敵との衝突判定
		float distPenetrationEnemy = sqrtf((PenetrationPosition.position.x - EnemyPosition.position.x) * (PenetrationPosition.position.x - EnemyPosition.position.x) +
			(PenetrationPosition.position.y - EnemyPosition.position.y) * (PenetrationPosition.position.y - EnemyPosition.position.y));

		float distReflectionEnemy = sqrtf((ReflectionPosition.position.x - EnemyPosition.position.x) * (ReflectionPosition.position.x - EnemyPosition.position.x) +
			(ReflectionPosition.position.y - EnemyPosition.position.y) * (ReflectionPosition.position.y - EnemyPosition.position.y));

		//if (distPenetrationEnemy <= PenetrationPosition.radius + EnemyPosition.radius)
		//{
		//	// 貫通が敵に衝突したら何もしない
		//}

		//if (distReflectionEnemy <= ReflectionPosition.radius + EnemyPosition.radius)
		//{
		//	// 反射が敵に衝突したら反射する
		//	Vector2 toEnemy = {EnemyPosition.position.x - ReflectionPosition.position.x, EnemyPosition.position.y - ReflectionPosition.position.y};
		//	float len = sqrtf(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
		//	toEnemy.x /= len;
		//	toEnemy.y /= len;

		//	ReflectionPosition.velocity.x = -ReflectionPosition.velocity.x + 2 * (ReflectionPosition.velocity.x * toEnemy.x + ReflectionPosition.velocity.y * toEnemy.y) * toEnemy.x;
		//	ReflectionPosition.velocity.y = -ReflectionPosition.velocity.y + 2 * (ReflectionPosition.velocity.x * toEnemy.x + ReflectionPosition.velocity.y * toEnemy.y) * toEnemy.y;

		//	// 反射する方向を計算（反射角＝入射角となるように計算）
		//	float dotProduct = ReflectionPosition.velocity.x * toEnemy.x + ReflectionPosition.velocity.y * toEnemy.y;
		//	ReflectionPosition.velocity.x = ReflectionPosition.velocity.x - 2 * dotProduct * toEnemy.x;
		//	ReflectionPosition.velocity.y = ReflectionPosition.velocity.y - 2 * dotProduct * toEnemy.y;
		//}
		
		if (distPenetrationEnemy <= PenetrationPosition.radius + EnemyPosition.radius) {
			// 貫通が敵に衝突したら色を赤にしタイマーをセット
			EnemyPosition.color = RED;
			EnemyPosition.hitTimer = 10;
		}

		if (distReflectionEnemy <= ReflectionPosition.radius + EnemyPosition.radius) {
			// 反射が敵に衝突したら反射する
			Vector2 toEnemy = { EnemyPosition.position.x - ReflectionPosition.position.x, EnemyPosition.position.y - ReflectionPosition.position.y };
			float len = sqrtf(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
			toEnemy.x /= len;
			toEnemy.y /= len;

			ReflectionPosition.velocity.x = -ReflectionPosition.velocity.x + 2 * (ReflectionPosition.velocity.x * toEnemy.x + ReflectionPosition.velocity.y * toEnemy.y) * toEnemy.x;
			ReflectionPosition.velocity.y = -ReflectionPosition.velocity.y + 2 * (ReflectionPosition.velocity.x * toEnemy.x + ReflectionPosition.velocity.y * toEnemy.y) * toEnemy.y;

			// 反射する方向を計算（反射角＝入射角となるように計算）
			float dotProduct = ReflectionPosition.velocity.x * toEnemy.x + ReflectionPosition.velocity.y * toEnemy.y;
			ReflectionPosition.velocity.x = ReflectionPosition.velocity.x - 2 * dotProduct * toEnemy.x;
			ReflectionPosition.velocity.y = ReflectionPosition.velocity.y - 2 * dotProduct * toEnemy.y;

			// 反射が敵に衝突したら色を赤にしタイマーをセット
			EnemyPosition.color = RED;
			EnemyPosition.hitTimer = 10;
		}

		// ヒットタイマーをデクリメント
		if (EnemyPosition.hitTimer > 0) {
			EnemyPosition.hitTimer--;
			if (EnemyPosition.hitTimer == 0) {
				EnemyPosition.color = WHITE; // 元の色に戻す
			}
		}
		
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		/// 
		Novice::DrawEllipse((int)PenetrationPosition.position.x, (int)PenetrationPosition.position.y, (int)PenetrationPosition.radius, (int)PenetrationPosition.radius, 0.0f, BLUE, kFillModeSolid); // 貫通

		Novice::DrawEllipse((int)ReflectionPosition.position.x, (int)ReflectionPosition.position.y, (int)PenetrationPosition.radius, (int)ReflectionPosition.radius, 0.0f, RED, kFillModeSolid); // 反射

		// 敵の描画
		Novice::DrawEllipse((int)EnemyPosition.position.x, (int)EnemyPosition.position.y, (int)EnemyPosition.radius, (int)EnemyPosition.radius, 0.0f, EnemyPosition.color, kFillModeSolid);

		///
		///
		/// ↑描画処理ここまで
		///		  

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}