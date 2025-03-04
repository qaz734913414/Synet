
function TEST {
FRAMEWORK=$1
NAME=$2
NUMBER=$3
THREAD=$4
FORMAT=$5
BATCH=$6
VERSION=$7
DIR=./data/"$FRAMEWORK"/"$NAME"
PATHES="-om=$DIR/other.dsc -ow=$DIR/other.dat -sm=$DIR/synet.xml -sw=$DIR/synet.bin -id=$DIR/image -od=$DIR/output -tp=$DIR/param.xml"
PREFIX="${FRAMEWORK:0:1}"
LOG=./test/"$FRAMEWORK"/"$NAME"/"$PREFIX"_"$NAME"_t"$THREAD"_f"$FORMAT"_b"$BATCH"_v"$VERSION".txt

echo $LOG

if [ -f $DIR/image/descript.ion ]
then
	rm $DIR/image/descript.ion
fi

./build_"$FRAMEWORK"/test_"$FRAMEWORK" -m=convert $PATHES -tf=$FORMAT

./build_"$FRAMEWORK"/test_"$FRAMEWORK" -m=compare -e=3 $PATHES -if=*.ppm -rn=$NUMBER -wt=1 -tt=$THREAD -bs=$BATCH -t=0.001 -dp=0 -dpf=5 -dpl=2 -ar=0 -rt=0.3 -ln=$LOG

}

if [ ! -d ./test ];then
	mkdir ./test
fi

#TEST darknet test_000 5 1 1 1 001a

TEST inference_engine test_000 500 1 1 1 003a
TEST inference_engine test_001 500 1 1 1 004a
TEST inference_engine test_002 20 1 1 1 003a
TEST inference_engine test_003f 50 1 1 1 002a
#TEST inference_engine test_003i 5 1 1 1 002a
TEST inference_engine test_004 200 1 1 1 002a
TEST inference_engine test_005 2000 1 1 1 001a
TEST inference_engine test_006 100 1 1 1 001a
TEST inference_engine test_007 200 1 1 1 002a

exit
