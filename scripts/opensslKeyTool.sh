#!/bin/bash
#
# Tool to manage openssl keys
#
keyDir=~/.ssh
commonName=$(whoami)

keyInfoFile=~/.irvinecubesat.keyInfo
if [ -e "$keyInfoFile" ]; then
    # shellcheck disable=SC1090
    . "$keyInfoFile"
fi

log()
{
    echo "$*"
}

usage()
{
    cat <<EOF
Usage:  $0 [options] {key file(s)} ...

        Tool to create/utilize openssl certificates and public keys
        for public key cryptography

Options
    -g {keyName} Generate a new self-signed cert with public/private keys
    -e {file}    Encrypt the input file with public certifcates  
    -d {file}    decrypt the input file with private key 
    -o {file}    specify the output file (Default stdout)
    -k {dir}     Keystore directory (Default: $keyDir)
    -c {CN}      The common name (CN) for cert generation(Default:  ${commonName}))
    -n {keyName} Use keyname (Default:  $keyName)
    -p           Extract the public key from the cert
    -f {cfgfile} File to store cert params/initialze params
EOF
    exit 1
}


unset outputFile

while getopts "g:e:d:k:n:c:g:o:i:f:ph" arg; do
    case $arg in
        g)
            cmd="gen"
            keyName=$OPTARG
            ;;
        e)
            cmd="enc"
            inputFile=$OPTARG
            ;;
        d)
            cmd="dec"
            inputFile=$OPTARG
            ;;
        c)
            commonName=$OPTARG
            ;;
        n)
            keyName=$OPTARG
            ;;
        k)
            keyDir=$OPTARG
            ;;
        o)
            outputFile=$OPTARG
            ;;
        f)
            cfgFile=$OPTARG
            if [ -n "$cfgFile" ] && [ -f "$cfgFile" ]; then
                # shellcheck disable=SC1090
                . "$cfgFile"
            fi
            ;;
        p)
            cmd="pubkey"
            ;;
        h)
            usage
            ;;
        *)
            usage
            ;;
    esac
done
shift "$((OPTIND-1))"

if [ -n "$outputFile" ]; then
    outputArg="-out $outputFile"
fi

case $cmd in
    gen)
        if [ -n "$cfgFile" ] && [ -n "${keyName}" ] && [ -n "$keyDir" ]; then
            {
                echo "keyName=${keyName}"
                echo "keyDir=${keyDir}"
            } > "$cfgFile"
        fi
        mkdir -p "${keyDir}"
        chmod 700 "${keyDir}"
        keyPath="${keyDir}/${keyName}.key"
        openssl req -x509 -newkey rsa:4096 -days 3650 -nodes -subj "/C=US/ST=*/L=*/O=*/OU=*/CN=${commonName}/" -keyout "$keyPath" -out "${keyDir}/${keyName}.cert"
        exitStatus=$?
        if ! chmod 600 "$keyPath"; then
            log "[E] Unable to set permissions on $keyPath"
        fi
    ;;
    enc)
        if [ -n "$*" ]; then
            keys="$*"
        else
            keys="${keyDir}/${keyName}.cert"
        fi
        # shellcheck disable=SC2086
        openssl smime -encrypt -aes256 -in "$inputFile" $outputArg -outform PEM $keys
        exitStatus=$?
        ;;
    dec)
        if [ -n "$1" ]; then
            key="$1"
        else
            key=${keyDir}/${keyName}.key
        fi

        # shellcheck disable=SC2086
        openssl smime -decrypt -in "$inputFile" -inform PEM -inkey "$key" $outputArg
        exitStatus=$?
        if [ $exitStatus -ne 0 ]; then
            log "[E] Unable to decrypt $inputFile with $key"
            # cleanup output file
            if [ -n "$outputFile" ]; then
                rm "$outputFile"
            fi
        fi
        ;;
    pubkey)
        key=${keyDir}/${keyName}.key
        cert=${keyDir}/${keyName}.cert
        pubkey=${key}.pub
        openssl x509 -pubkey -noout -in "$cert"|ssh-keygen -f /dev/stdin -i -m PKCS8|sed -e "1 s/\$/ $keyName/">"$pubkey"
        ;;
    *)
        log "[E] Unknown cmd:  $cmd"
        exit 1
        ;;
esac

exit $exitStatus

