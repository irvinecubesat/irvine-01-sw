#!/bin/sh

DEPLOY_DIR=/data/deployment
# I had issues with that directory not existing,
#  but I didn't want to create it here since I 
#  use it for the loop below, so for now these are
#  hard-coded paths.  They'll probably be fixed
#  later
DEPLOY_LOG=$DEPLOY_DIR/deploy.log
ANTENNA_FLAG=$DEPLOY_DIR/antenna_deployed
SATCOMM_FLAG=$DEPLOY_DIR/satcomm_enabled
DSA_DEPLOYED_FLAG=$DEPLOY_DIR/dsa_deployed
ELAPSED_TIME=$DEPLOY_DIR/elapsed_time_in_seconds
ANTENNA_DEPLOY_TIME=$DEPLOY_DIR/antenna_deploy_time
ALL_DEPLOY_COMPLETE_FLAG=$DEPLOY_DIR/deploy_complete

# INFO:
#    Times are listed in <orbits>*<minutes_per_orbit>*<seconds>

# Need to implement DSA deployment

# In the event of sudden instabilty in orbit, there
#  are two ways in which the elapsed time can be
#  updated.
#     1. When the script is first run, the time
#        increments by 30 seconds if the antenna
#        deployment flag has already been set
#     2. The normal method, where this script
#        increments the time every second

# First action should be to deploy the antenna
#  with the limited time this script potentially has

# Log function, can be replaced with a better
#  implementation
log() {
    echo "`date`: $1" >> $DEPLOY_LOG
}

# get the elapsed time since initial deployment
getTime() {
    echo "`cat $ELAPSED_TIME`"
}

# increment the time elapsed since initial deployment
incrementTime() {
    echo "`getTime`" > $ELAPSED_TIME
}            

#log "starting deployment"

# Start by creating the directory for all the 
#  deployment flags, setting intial values,
#  and taking the first picture
if [ ! -d $DEPLOY_DIR ]; then
    mkdir -p $DEPLOY_DIR
    log "created deployment directory"
    echo "0" > $ANTENNA_FLAG
    echo "0" > $SATCOMM_FLAG
    echo "0" > $ELAPSED_TIME
    echo "0" > $DSA_DEPLOYED_FLAG
    echo "0" > $ALL_DEPLOY_COMPLETE_FLAG
    log "finished setting initial flags"

    CURDIR=$PWD
    cd /sdcard
    uvccapture -M -j -x2048 -y1536 -oinitialPicture
    cd $CURDIR
fi

while [ `cat $ALL_DEPLOY_COMPLETE_FLAG` -ne 1 ]; do 
    # Increment the time every second
    sleep 1
    incrementTime
    # If it hasn't been one orbit since the antenna
    #  was deployed, disable satcomm to avoid issues
    #  with the antenna shorting the board
    if ( [ `cat $ANTENNA_FLAG` -ne 1 ] || [ ( getTime - `cat $ANTENNA_DEPLOY_TIME` ) -le 2*90*60 ] ) ; then
        log "disabling satcomm"
        #rm /critical_state/satcom*
    fi

    # Antenna deployment proceedure, occurs after a
    #  full orbit at Cal Poly's recommendation
    if [ `cat $ANTENNA_FLAG` -ne 1 && getTime -ge 1*90*60 ]; then
        log "Deploying antenna" 
        # sys-deploy-now -n 20
        sleep 20
        log "Antenna deployed at `getTime`"
        echo "1" > $ANTENNA_FLAG
        echo getTime > $ANTENNA_DEPLOY_TIME

        CURDIR=$PWD
        cd /sdcard
        uvccapture -M -j -x2048 -y1536 -oantennaDeployedPhoto
        cd $CURDIR
    fi

    # Enable satcomm communication if the antennas have been deployed
    if [ `cat $ANTENNA_FLAG` -eq 1 ]; then
        log "Enabling satcomm"
        #satcomm-unsquelch
        #satcomm-1st-inhibit-off
        #satcomm-2nd-inhibit-off
    fi

    # This needs to be completed and the time delay set to the 
    #  proper value
    if [ `getTime` -ge 1*90*60 ]; then
        log "Deploying DSA's"
        #PUTDEPLOYSCRIPTHERE
        # Set the DSA DEPLOY FLAG to 1 once finished
    fi
    
    # This checked if all the deployments have succeeded and if
    #  so exits the deployment script
    CAN_DEPLOY=1
    for FILE in $ANTENNA_FLAG $SATCOMM_FLAG $DSA_DEPLOYED_FLAG; do
        CAN_DEPLOY=( $CAN_DEPLOY * `cat $(FILE) )
    done
    if [ $CAN_DEPLOY -eq 1 ]; then
        log "Deployment complete"
        echo "1" > $ALL_DEPLOY_COMPLETE_FLAG
    fi
done




