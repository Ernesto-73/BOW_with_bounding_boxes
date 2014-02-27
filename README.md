#bow_bp
#By Javier Zhang

项目主页：http://git.oschina.net/javier/bow_bp

这个项目是我很久以前就打算做的，正式开始实在年前12月份，因为工作上的原因，平时很少有时间花在这个项目上，一拖再拖，终于在2月底完成了一个成熟的版本（3.x）
其实早在09年，opencv更新了BOW类以后，就有人在codeproject上发布过类似的代码；不过，我在完成这个项目的时候，并没有参考其它任何人的源代码。

算法完全依照论文中的思路，唯一的不同是，使用了bounding-box，对特征点提取和匹配的范围进行了限制。

实验选用了Caltech-101数据库：
http://www.vision.caltech.edu/Image_Datasets/Caltech101/
数据库中，带有annotations，全部都是mat文件（老古董了，现在大多使用xml）

之所以选择在linux下实现该算法，主要是本人对libxml++和matio库有所了解，而这两个库目前还不能在windows下使用。

代码依赖库：
libxml
libxml++
libmatio
opencv
openmp

- 编译：
/bow_bp$ make
/bow_bp$ make bow

1. 通过makfile可以配置编译选项，-g -wall -D DEBUG打开调试模式.
2. 想使用PCA算法的可以通过-D _USE_PCA_（我感觉使用PCA对结果帮助不大）
3. 不想使用openmp可把-D _USE_OPENMP_ 

- 运行：
/bow_bp$ ./bin/bow -p /home/javier/Caltech-101/ -e SIFT -c BP -C Kmeans++ -n 50

- 配置文件：
当然，也可以使用配置文件，使用命令行参数运行一次以后就不需要在使用命令行参数了，直接配置自动生成的box.xml文件就可以了
配置文件实例：

	<BOW_Configurations>
	<path>/home/javier/Caltech-101/</path>
	<annotationsPath>/home/javier/Caltech-101//Annotations</annotationsPath>
	<imagesPath>/home/javier/Caltech-101//Images</imagesPath>
	<extractor>SIFT</extractor>
	<detector>SIFT</detector>
	<classifier>BP</classifier>
	<cluster>Kmeans++</cluster>
	<numClusters>42</numClusters>
	</BOW_Configurations>

- 实验：
shell脚本run.sh适用于批量实验(shell脚本我不是很熟悉，写的不好)
进行批量实验前，需要将conf.cpp的135-142行注释掉，然后重新编译。

该项目实际还没有完成，包括添加GMM模型和Harris特征

有兴趣的同学可以跟我一起完善该项目（发送merge请求前，最好先联系我）
有什么问题或者发现了bug，请联系我： zhang_2348@126.com

- Happy life, happy coding.

关注我的其他项目：
http://git.oschina.net/javier



