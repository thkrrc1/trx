# ROS2 trxセットアップ

## 参考
- [ROS2 JazzyJalisco : Installation Ubuntu (Debian packages)](https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html)

## 1.事前準備
1. udev設定ファイル作成<br>
    /etc/udev/90-aero.rulesを作成し、下記のとおり記入する。
    ```
    $ sudo nano /etc/udev/rules.d/90-aero.rules 

    【記入内容】
    SUBSYSTEM=="tty", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="a1e8", SYMLINK+="trx_s", MODE="666"
    ```
    記入後、udev設定の適応
    ```
    $ sudo udevadm control --reload-rules
    $ sudo udevadm trigger
    ```

1. 全てのパッケージをupgradeする。
    ```
    $ sudo apt update
    $ sudo apt upgrade
    ```
2. pip3インストール
    ```terminal
    $ sudo apt install python3-pip
    ```

## 2. ROS2 jazzyインストール
公式サイトの手順に従いROS2 jazzyをインストールする。
1. インストール  
[ROS2 jazzy : Installation Ubuntu (Debian packages)](https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html)

1. 環境設定  
[Configuring environment](https://docs.ros.org/en/jazzy/Tutorials/Beginner-CLI-Tools/Configuring-ROS2-Environment.html)  
※「3.1 The ROS_DOMAIN_ID variable」の設定は不要。

1. ビルドの確認  
[Using colcon to build packages](https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Colcon-Tutorial.html)  
1. colcon cleanをインストール
    ```terminal
    $ sudo apt install python3-colcon-clean 
    ```
1. rosdep初期化
    ```terminal
    $ sudo rosdep init
    $ rosdep update
    ```
    ※colcon cleanをインストールすると以下のコマンドでビルドによる出力内容を削除することが出来る。<br> 
    ワークスペース以外で誤ってビルドを行った場合などビルド結果を削除するのに便利。
    ```
    $ colcon clean workspace
    ```

## 3.trxインストール
1. ワークスペース作成 
    (ワークスペース名は任意に設定することが可能だが、ここでは"ros2_ws"とする。)
    ```
    $ mkdir -p ~/ros2_ws/src
    ```
1. trxパッケージインストール
    ```terminal
    $ cd ~/ros2_ws/src
    $ git clone git@github.com:thkrrc1/trx.git
    ```
1. その他パッケージインストール
    ```
    $ sudo apt install ros-jazzy-ros2-control ros-jazzy-controller-manager
    $ sudo apt install ros-jazzy-joint-state-broadcaster 
    $ sudo apt install ros-jazzy-joint-trajectory-controller
    ```
1. rqt本体とjoint_trajectory_controllerプラグインのインストール
    ```
    $ sudo apt install ros-jazzy-rqt
    $ sudo apt install ros-jazzy-rqt-joint-trajectory-controller
    ```
1. ビルド
    ```
    $ cd ~/ros2_ws
    $ colcon build --symlink-install
    ```

## 4.実行
1. ロボットモデル起動
    ```terminal
    $ cd ~/ros2_ws
    $ source install/setup.bash
    $ ros2 launch trx bringup.launch.py
    ```
    Rviz2に下記画像のようなtrxのモデルが表示されればok.
    ![trx_model](https://github.com/user-attachments/assets/9c5e7f8b-ead5-4308-aa24-848e36354fdc)
<br><br>
1. コントローラ起動

    別ターミナルを開き、
    ```terminal
    $ ros2 run rqt_joint_trajectory_controller rqt_joint_trajectory_controller 
    ```
    下記画像のようにrqtのGUIが起動し、jointsのスライダーを動かして動作することが確認できればok.
    ![rqt＿GUI](https://github.com/user-attachments/assets/eb2d4a19-3fc8-461d-a316-50dc0c0b4c59)

## ※諸注意
1. ID変更対応<br>
   制御するtrxのモータードライバー(SEED MC)のIDを把握した上で、ソースコード内の下記パラメータの記述を適宜修正してください。
   ```
   /trx/urdf/trx_s.urdf 内の記述
              ︙
   <ros2_control name="SeedHW" type="system">
    <hardware>
      <plugin>trx/SeedHardwareInterface</plugin>
      <param name="serial_port">/dev/trx_s</param>
      <param name="can_id">1</param>   ＜ーーーーーーーー制御するIDに合わせる（この場合は ID 1）
      <param name="controller_rate">20.0</param>
    </hardware>
              ︙
2. 依存パッケージ不具合（26/5/15 時点）<br>
   ros-jazzy-joint-trajectory-controller、ros-jazzy-joint-state-broadcasterのパッケージバージョン v4.39において、Nodeが正常に起動しない不具合を確認しております。
   構築環境にてこれらに起因する不具合を確認した場合、各パッケージ 推奨バージョン v4.36にダウングレードした上で、本パッケージを再度ビルドして動作をお試しください。

以上
