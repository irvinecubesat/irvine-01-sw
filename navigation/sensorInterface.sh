i=0
usage() {
	echo "Try ./sensorInterface {args} e.g.: './sensorInterface Mag_X Accel_Y Gyro_Z Accel_X'"
}

#check if script is called with empty args or -h help arg and provide usage
if [ -z "$1"  ]
then 
		usage
		exit 1
fi

if [ $1 == "-h" ]
then
		usage
		exit 1
fi

#loop through args, check for specific cases and add these to array with predetermined int code
for t in "$@"
do
	case $t in
		"Mag_X")
			ca[$i]=6
			;;
		"Mag_Y")
			ca[$i]=7
			;;
		"Mag_Z")
			ca[$i]=8
			;;
		"Accel_X")
			ca[$i]=0
			;;
		"Accel_Y")
			ca[$i]=1
			;;
		"Accel_Z")	
			ca[$i]=2
			;;
		"Gyro_X")
			ca[$i]=3
			;;
		"Gyro_Y")
			ca[$i]=4
			;;
		"Gyro_Z")
			ca[$i]=5
			;;
		*)
			echo "Error in inputs, refer to example using 'sensorInterface -h'"
			exit 1
	esac
	((i++))
done

#Loop through array and generate string with arguments to use with CPP backend file
strargs=""
for e in ${ca[*]};
do
	strargs=$strargs$e
	strargs="$strargs "
done

#echo "$strargs"

#Call backend code with given args
./sensorManager $strargs
