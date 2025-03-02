#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// 動畫幀結構體，用於儲存動畫的每一幀的紋理和相關資訊
typedef struct AnimFrame {
    Texture2D tex; // 幀紋理
    int cellW; // 單個動畫單元格寬度
    int cellH; // 單個動畫單元格高度
    int centerW; // 單元格中心 X 偏移量
    int centerH; // 單元格中心 Y 偏移量
    int xCellCount; // 紋理在 X 軸方向上的單元格數量
    int yCellCount; // 紋理在 Y 軸方向上的單元格數量
} AnimFrame;

// 載入動畫幀
// fname: 紋理圖片文件名
// cell_width: 動畫單元格寬度
// cell_height: 動畫單元格高度
AnimFrame AnimFrameLoad(const char* fname, uint16_t cell_width, uint16_t cell_height)
{
    AnimFrame a = { 0 }; // 初始化動畫幀結構體

    a.tex = LoadTexture(fname); // 使用 raylib 載入紋理
    if (a.tex.id == 0) {
        printf("Error: Failed to load texture from %s\n", fname); // 載入失敗時輸出錯誤信息
        return a; // 返回空的動畫幀
    }
    // 注意：cell_width 和 cell_height 為無符號整數，僅檢查是否為 0
    if (cell_width == 0 || cell_height == 0) {
        a.cellW = 0;
        a.cellH = 0;
    } else {
        a.cellW = cell_width; // 設定單元格寬度
        a.centerW = a.cellW >> 1; // 計算中心 X 偏移量 (等同除以2)
        a.cellH = cell_height; // 設定單元格高度
        a.centerH = a.cellH >> 1; // 計算中心 Y 偏移量 (等同除以2)
        a.xCellCount = a.tex.width / a.cellW; // 計算 X 軸上的單元格數量
        a.yCellCount = a.tex.height / a.cellH; // 計算 Y 軸上的單元格數量
    }
    return a; // 返回載入的動畫幀
}

// 卸載動畫幀，釋放紋理資源
void AnimFrameUnload(AnimFrame* af)
{
    UnloadTexture(af->tex); // 使用 raylib 卸載紋理，釋放內存
}

// 方向枚舉（目前未在本示例中使用，可作擴展用）
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
} Direction;

// 史萊姆姿態枚舉，定義精靈的不同狀態
typedef enum {
    IDLE = 0, // 靜止
    MOVE, // 移動
    END_POSE, // 姿態結束標記（用於計算紋理中的行偏移）
} SlimePose;

// 精靈結構體
typedef struct {
    int id; // 精靈 ID（用於計算在 sprite sheet 中的所在行）
    AnimFrame af; // 動畫幀數據
    SlimePose pose; // 當前姿態（IDLE 或 MOVE）
    int frame_counter; // 每幀停留的計數器（目前未使用，可作速度調整用）
    int curr_frame; // 當前動畫幀索引
    float next_frame_time; // 切換到下一幀所需時間（秒）
    float frame_time; // 幀時間累計
    bool animate; // 是否播放動畫
    bool centor_cord; // 是否使用中心坐標繪製（建議變數命名改為 center_coord，可提高可讀性）
} Sprite;

// 載入精靈動畫幀數據（僅進行數據賦值，可擴展其他初始化）
void SpriteLoad(Sprite* s, AnimFrame af)
{
    s->af = af; // 將動畫幀數據賦值給精靈
}

// 史萊姆顏色枚舉（用來索引不同顏色的史萊姆，這裡定義的順序與精靈陣列 Slime 對應）
typedef enum {
    SC_AQUAMARINE = 0, // 海藍寶石色
    SC_BLUE, // 藍色
    SC_BLUEGREEN, // 藍綠色
    SC_DARKBLUE, // 深藍色
    SC_GOLD, // 金色
    SC_GREEN, // 綠色
    SC_LIGHTBLUE, // 淺藍色
    SC_MAROON, // 栗色
    SC_ORANGE, // 橙色
    SC_PALEGREEN, // 淡綠色
    SC_PINK, // 粉色
    SC_PURPLE, // 紫色
    SC_RED, // 紅色
    SC_VIOLET, // 紫羅蘭色
    SC_TOTAL, // 顏色總數（即精靈總數）
} SlimeColor;

// 宣告史萊姆精靈陣列，包含不同顏色的史萊姆精靈數據
Sprite Slime[SC_TOTAL] = {
    // 以下用指定初始化方式初始化各個精靈數據
    { .id = 0, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 1, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 2, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 3, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 4, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 5, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 6, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 7, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 8, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 9, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 10, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 11, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 12, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
    { .id = 13, .pose = IDLE, .frame_counter = 16, .curr_frame = 0, .next_frame_time = 1.0f / 16.0f, .frame_time = 0.0f, .animate = true, .centor_cord = true },
};

// 更新精靈動畫幀（根據計時器切換動畫幀）
void SpriteUpdate(Sprite* s)
{
    if (s->animate) { // 如果允許動畫播放
        s->frame_time += GetFrameTime(); // 累計經過的時間
        if (s->frame_time >= s->next_frame_time) { // 當累計時間超過下一幀所需時間
            s->frame_time = 0.0f; // 重置累計時間
            s->curr_frame++; // 切換到下一幀
            // 如果超過總幀數則循環回第一幀
            if (s->curr_frame >= (s->af.xCellCount * s->af.yCellCount)) {
                s->curr_frame = 0;
            }
        }
    }
}

// 繪製精靈到螢幕上
// s: 要繪製的精靈
// position: 繪製位置（通常為精靈中心點或左上角，取決於 centor_cord 設定）
void SpriteDraw(Sprite* s, Vector2 position)
{
    if (s->af.tex.id > 0) { // 確保紋理已成功載入
        // 計算紋理中 Y 軸的偏移量：根據精靈的 id 與姿態選擇對應的行
        // 注意：這裡假設每個精靈有 END_POSE 個姿態，且所有精靈的動畫排列在同一個 sprite sheet 中
        float pose_frame = (((s->id * END_POSE + s->pose) % s->af.yCellCount)) * s->af.cellH;

        // 計算當前幀在紋理中的矩形區域（來源區域）
        Rectangle sourceRec = {
            (float)(s->curr_frame % s->af.xCellCount) * s->af.cellW, // X 軸偏移量（根據列數計算）
            pose_frame, // Y 軸偏移量（根據姿態行計算）
            (float)s->af.cellW, // 單元格寬度
            (float)s->af.cellH // 單元格高度
        };
        // 設定目標繪製區域（預設以左上角為定位點）
        Rectangle destRec = {
            position.x,
            position.y,
            (float)s->af.cellW,
            (float)s->af.cellH
        };
        // 預設原點設為 (0, 0)
        Vector2 origin = { 0, 0 };
        // 如果設定使用中心坐標繪製，則調整原點及繪製位置
        if (s->centor_cord) {
            origin = (Vector2) { (float)s->af.centerW, (float)s->af.centerH };
            // 調整目標區域，使 position 為精靈中心
            destRec.x = position.x - s->af.centerW;
            destRec.y = position.y - s->af.centerH;
        }
        // 使用 raylib 的 DrawTexturePro 函數繪製部分紋理到螢幕上
        DrawTexturePro(s->af.tex, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

int main()
{
    // 初始化 raylib 視窗
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib sprite system demo"); // 創建視窗並設定標題
    SetTargetFPS(60); // 設定目標幀率為 60fps

    // 載入動畫幀，假設 sprite sheet 檔案位於 "img/slime.png"，單元格大小為 12 x 14
    AnimFrame slimeAF = AnimFrameLoad("img/slime.png", 12, 14);
    // 檢查動畫幀是否載入成功
    if (slimeAF.tex.id == 0) {
        CloseWindow(); // 載入失敗則關閉視窗並退出
        return 1;
    }

    // 為所有史萊姆精靈載入相同的動畫幀數據
    for (int i = 0; i < SC_TOTAL; i++) {
        SpriteLoad(&Slime[i], slimeAF);
    }
    Vector2 slimePositions[SC_TOTAL]; // 儲存每個史萊姆的繪製位置
    // 初始化史萊姆精靈位置，水平排列
    for (int i = 0; i < SC_TOTAL; i++) {
        slimePositions[i] = (Vector2){ 100.0f + i * 50.0f, 200.0f };
    }

    float moveSpeed = 5.0f; // 定義移動速度

    // 遊戲主循環
    while (!WindowShouldClose()) // 當視窗未要求關閉時持續執行
    {
        // 更新輸入及狀態
        if (IsKeyPressed(KEY_SPACE)) { // 當按下空白鍵時，切換所有史萊姆的姿態
            for (int i = 0; i < SC_TOTAL; i++) {
                if (Slime[i].pose == IDLE) {
                    Slime[i].pose = MOVE; // 從靜止切換到移動
                } else {
                    Slime[i].pose = IDLE; // 反之則切回靜止
                }
            }
        }

        // 根據方向鍵控制所有史萊姆的位置
        if (IsKeyDown(KEY_RIGHT))
        {
            for (int i = 0; i < SC_TOTAL; i++) {
                slimePositions[i].x += moveSpeed;
            }
        }
        if (IsKeyDown(KEY_LEFT))
        {
            for (int i = 0; i < SC_TOTAL; i++) {
                slimePositions[i].x -= moveSpeed;
            }
        }
        if (IsKeyDown(KEY_UP))
        {
            for (int i = 0; i < SC_TOTAL; i++) {
                slimePositions[i].y -= moveSpeed;
            }
        }
        if (IsKeyDown(KEY_DOWN))
        {
            for (int i = 0; i < SC_TOTAL; i++) {
                slimePositions[i].y += moveSpeed;
            }
        }

        // 更新所有史萊姆的動畫幀
        for (int i = 0; i < SC_TOTAL; i++) {
            SpriteUpdate(&Slime[i]);
        }

        // 繪製部分
        BeginDrawing();
            ClearBackground(BLACK);
            // 繪製所有史萊姆精靈及其編號
            for (int i = 0; i < SC_TOTAL; i++) {
                SpriteDraw(&Slime[i], slimePositions[i]);
                DrawText(TextFormat("%d", i), (int)slimePositions[i].x - 20, (int)slimePositions[i].y - 30, 20, GRAY);
            }
            DrawText("Simple sprite system with Raylib", 10, 10, 20, WHITE);
        EndDrawing();
    }

    // 卸載資源並關閉視窗
    AnimFrameUnload(&slimeAF);
    CloseWindow();

    return 0;
}
