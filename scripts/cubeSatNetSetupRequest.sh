#!/bin/bash
instdir=$(cd $(dirname "$0"); pwd)
topdir=${instdir}/..

OLD_KEYINFO_FILE=~/.irvine-01.keyInfo
KEYINFO_FILE=~/.irvinecubesat.keyInfo
CUBESATNET_SETUP_REQUEST=cubesatSetupRequest.txt
CUBESAT_ADMIN_CERT=auth/cubesat-admin.cert
KEYTOOL=$topdir/scripts/opensslKeyTool.sh

log()
{
    echo $*
}

#
# In some cases if you interrupt the script, echo will be turned off.
# Ensure that echo is turned on.
#
reEnableEcho()
{
    stty echo
}

trap reEnableEcho EXIT

if [ -e "$OLD_KEYINFO_FILE" ]; then
    log "[I] Converting $OLD_KEYINFO_FILE to $KEYINFO_FILE"
    mv "$OLD_KEYINFO_FILE" "$KEYINFO_FILE"
    if [ $? -ne 0 ]; then
	log "[E] Unable to move $OLD_KEYINFO_FILE to  $KEYINFO_FILE"
    fi
fi

while [ ! "ok" = "$ok" ]; do
    echo "Enter a secure password which you will use to access CubeSatNet:" 
    read -s password;
    echo "Confirm your password by typing it again:"
    read -s confirmPassword 
    if [ "$confirmPassword" = "$password" ]; then 
	ok="ok" 
	echo "Show your password? (y/N)"
	read displayPw
	if [ "$displayPw" = "y" ] || [ "$displayPw" = "Y" ]; then
	    echo
	    echo "Your password is:  $password"
	    echo
	    echo "If you don't like it, rerun this script."
	    echo
	fi
    else
	echo
	echo "Passwords did not match"
	echo
    fi
done

tmpFile=$(mktemp)
if [ ! -e "$KEYINFO_FILE" ]; then
    log "[E] Unable to find $KEYINFO_FILE.  Did you run make genKeys?"
fi
. "${KEYINFO_FILE}";

echo "$password">$tmpFile
setupRequestFile=$keyName-${CUBESATNET_SETUP_REQUEST}
encryptedFile=${setupRequestFile}.enc
cat "$tmpFile" ~/.ssh/"${keyName}.cert" >"$setupRequestFile"

${KEYTOOL} -e "$setupRequestFile" -o "$encryptedFile" "${CUBESAT_ADMIN_CERT}"
rm -f "$tmpFile" "$setupRequestFile"
echo "Please create a Jira issue at jira.irvinecubesat.com in the ICS project and attach $encryptedFile to the issue."
echo
echo "Note:  Changing your password in your openvpn config is a future feature :-)"
