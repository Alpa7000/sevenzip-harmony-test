import { hilog } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';
import testNapi from 'libentry.so';

const DOMAIN = 0x0000;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize($r('app.float.page_text_font_size'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            // 获取应用沙箱路径（推荐使用无参 getContext，API 9+）
            const context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
            const filesDir: string = context.filesDir;

            // 调用 Native add 函数，传入两个数值和沙箱路径
            const result: number = testNapi.add(2, 3, filesDir);
            this.message = `Result: ${result}`;
            hilog.info(DOMAIN, 'testTag', 'Test NAPI 2 + 3 = %{public}d', result);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}