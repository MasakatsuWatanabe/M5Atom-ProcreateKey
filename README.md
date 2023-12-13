# Procreate用お絵描き支援キーボード（1ボタン）

Procreate用の1ボタン外部キーボードを作成するプロジェクトです。
ボタンを押している間は消しゴム機能が、ボタンを放している間はブラシ機能が有効になります。
180秒間の無操作でディープスリープに入ります。スリープ時はボタンでレジューム（正確には再起動）します。

## 使用するハードウェア

以下のハードを使用します。
- **M5Atom Lite**: ESP32ベースの開発キット。詳細は[M5Stack公式ストア](https://shop.m5stack.com/products/atom-lite-esp32-development-kit)を参照。
- **Mechanical Key Button Unit (SKU: U144)**: 物理ボタンユニット。詳細は[M5Stack公式ストア](https://shop.m5stack.com/products/mechanical-key-button-unit)を参照。

## 開発環境

[PlarformIO](https://platformio.org/)を使用しています。
使用しているライブラリはplatform.iniを参照してください。

## 免責事項

- 作者はプロジェクトに関連するいかなる保証も行いません。
- プロジェクトの使用によって生じたいかなる損害について、作者は責任を負いません。
