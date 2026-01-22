#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkRRect.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"
#include "include/ports/SkFontMgr_fontconfig.h"
#include "include/ports/SkFontScanner_FreeType.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"

using namespace skia::textlayout;

int main() {
    // 创建位图和画布
    SkBitmap bitmap;
    bitmap.allocPixels(SkImageInfo::MakeN32Premul(400, 300));
    SkCanvas canvas(bitmap);
    canvas.clear(SK_ColorWHITE);

    // 1. 绘制填充矩形
    SkPaint rectPaint;
    rectPaint.setColor(SK_ColorBLUE);
    canvas.drawRect(SkRect::MakeXYWH(20, 20, 100, 60), rectPaint);

    // 2. 绘制圆角矩形
    SkPaint rrectPaint;
    rrectPaint.setColor(SK_ColorGREEN);
    canvas.drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(140, 20, 100, 60), 10, 10), rrectPaint);

    // 3. 绘制圆形
    SkPaint circlePaint;
    circlePaint.setColor(SK_ColorRED);
    canvas.drawCircle(320, 50, 30, circlePaint);

    // 4. 绘制线条
    SkPaint linePaint;
    linePaint.setColor(SK_ColorBLACK);
    linePaint.setStrokeWidth(3);
    linePaint.setStyle(SkPaint::kStroke_Style);
    canvas.drawLine(20, 100, 380, 100, linePaint);

    // 5. 绘制路径（三角形）
    SkPath path = SkPathBuilder()
        .moveTo(70, 200)
        .lineTo(120, 130)
        .lineTo(170, 200)
        .close()
        .detach();
    SkPaint pathPaint;
    pathPaint.setColor(SkColorSetARGB(255, 255, 128, 0));
    canvas.drawPath(path, pathPaint);

    // 6. 绘制描边矩形
    SkPaint strokePaint;
    strokePaint.setColor(SkColorSetARGB(255, 128, 0, 255));
    strokePaint.setStyle(SkPaint::kStroke_Style);
    strokePaint.setStrokeWidth(4);
    canvas.drawRect(SkRect::MakeXYWH(200, 130, 80, 70), strokePaint);

    // 7. 使用 skparagraph 绘制支持字体回退的文字
    sk_sp<SkFontMgr> fontMgr = SkFontMgr_New_FontConfig(nullptr, SkFontScanner_Make_FreeType());
    
    auto fontCollection = sk_make_sp<FontCollection>();
    fontCollection->setDefaultFontManager(fontMgr);
    fontCollection->enableFontFallback();

    ParagraphStyle paraStyle;
    TextStyle textStyle;
    textStyle.setColor(SK_ColorBLACK);
    textStyle.setFontSize(20);
    textStyle.setFontFamilies({SkString("sans-serif")});
    paraStyle.setTextStyle(textStyle);

    auto builder = ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText("Hello 你好 🎉");
    
    auto paragraph = builder->Build();
    paragraph->layout(360);
    paragraph->paint(&canvas, 20, 240);

    // 保存为 PNG
    SkFILEWStream file("skia_demo.png");
    SkPngEncoder::Encode(&file, bitmap.pixmap(), {});

    return 0;
}
