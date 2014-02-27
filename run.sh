#!bin/bash
# Author： Javier Zhang 
# bow experiments 

rm ./bow.xml >/dev/null 2>&1

echo 'Runing bow experiments...'

num=100

for((i=52;i<=$num;i=i+2));
do 
	echo "Num_centroid: $i"
	./bin/bow -p /home/javier/Caltech-101/ -e SIFT -c BP -C Kmeans++ -n $i > ./logs/ans_$i.txt
	rm ./bow.xml
done
