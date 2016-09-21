#!/bin/bash
#
# Tool to manage openssl keys
#
keyDir=~/.ssh
commonName=$(whoami)

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
    -o {file}    specify the output file (Default:  {input}.enc
    -k {dir}     Keystore directory (Default: $keyDir)
    -c {CN}      The common name (CN) for cert generation(Default:  ${commonName}))
    -f {cfgfile} File to store cert params/initialze params
EOF
    exit 1
}


while getopts "g:e:d:k:c:g:o:i:f:h" arg; do
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
        k)
            keyDir=$OPTARG
            ;;
        o)
            outputFile=$OPTARG
            ;;
        f)
            cfgFile=$OPTARG
            if [ -n "$cfgFile" ] && [ -f "$cfgFile" ]; then
                . "$cfgFile"
            fi
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

if [ -n "$cfgFile" ] && [ -n "${keyName}" ] && [ -n "$keyDir" ]; then
    {
        echo "keyName=${keyName}"
        echo "keyDir=${keyDir}"
    } > "$cfgFile"
fi

case $cmd in
    gen)
        mkdir -p "${keyDir}"
        chmod 700 "${keyDir}"
        openssl req -x509 -newkey rsa:4096 -days 3650 -nodes -subj "/C=US/ST=*/L=*/O=*/OU=*/CN=${commonName}/" -keyout "${keyDir}/${keyName}.key" -out "${keyDir}/${keyName}.cert"
        exitStatus=$?
    ;;
    enc)
        if [ -n "$*" ]; then
            keys="$*"
        else
            keys="${keyDir}/${keyName}.cert"
        fi
        outputFile=${outputFile-${inputFile}.enc}
        # shellcheck disable=SC2086
        openssl smime -encrypt -aes256 -in "$inputFile" -out "$outputFile" -outform PEM $keys
        exitStatus=$?
        ;;
    dec)
        if [ -n "$1" ]; then
            key="$1"
        else
            key=${keyDir}/${keyName}.key
        fi
        outputFile=${outputFile-${inputFile%.*}}

        openssl smime -decrypt -in "$inputFile" -inform PEM -inkey "$key" > "$outputFile"
        exitStatus=$?
        if [ $exitStatus -ne 0 ]; then
            log "[E] Unable to decript $inputFile with $key"
            # cleanup output file
            rm "$outputFile"
        fi
        ;;
    *)
        log "[E] Unknown cmd:  $cmd"
        exit 1
        ;;
esac

exit $exitStatus

