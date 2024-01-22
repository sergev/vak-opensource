#include <gpgme.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

void printkey(const char *title, char val[], unsigned nbytes)
{
    static const char ASCII64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@%0123456789";
    int i, reg, n;

    printf("%s ", title);

    reg = nbytes;
    n = 16;
    i = 0;
    for (;;) {
        putchar(ASCII64[reg & 077]);
        reg >>= 6;
        n -= 6;
        if (n < 6) {
            if (i >= nbytes) {
                break;
            }
            reg |= (unsigned char)val[i++] << n;
            n += 8;
        }
    }
    if (n > 0) {
        putchar(ASCII64[reg & 077]);
    }
    printf("\n");
}

void encode()
{
#if 0
    char inbuf[KEYSZ], outbuf[KEYSZ];
    int q, n, err;

    q = countbytes((void *)pn);
    for (;;) {
        n = read(0, inbuf, q - 4);
        if (n < 0)
            perror("input");
        if (n <= 0)
            break;
        err = rsa_public_encrypt((unsigned char *)outbuf, (unsigned char *)inbuf, n,
                                 (unsigned char *)pe, (unsigned char *)pn);
        if (err) {
            fprintf(stderr, "rsa_public_encrypt error %d\n", err);
            break;
        }
        if (write(1, outbuf, q) != q) {
            perror("output");
            break;
        }
    }
#endif
}

void decode()
{
#if 0
    char inbuf[KEYSZ], outbuf[KEYSZ];
    int q, n;

    q = countbytes((void *)sn);
    for (;;) {
        n = read(0, inbuf, q);
        if (n == 0)
            break;
        if (n != q) {
            perror("input");
            break;
        }
        n = rsa_private_decrypt((unsigned char *)outbuf, (unsigned char *)inbuf,
                                (unsigned char *)se, (unsigned char *)sd, (unsigned char *)sp,
                                (unsigned char *)sq, (unsigned char *)su, (unsigned char *)sn);
        if (n <= 0) {
            fprintf(stderr, "rsa_private_decrypt error %d\n", n);
            break;
        }
        if (write(1, outbuf, n) != n) {
            perror("output");
            break;
        }
    }
#endif
}

void cryptfile(char *key, int decodeflag)
{
#if 0
    char inbuf[512], outbuf[512];
    int q, n;

    q = 512;
    for (;;) {
        n = read(0, inbuf, q);
        if (n < 0)
            perror("input");
        if (n <= 0)
            break;
        if (decodeflag)
            block_decrypt(inbuf, outbuf, n, key);
        else
            block_encrypt(inbuf, outbuf, n, key);
        if (write(1, outbuf, n) != n) {
            perror("output");
            break;
        }
    }
#endif
}

void print_data(const char *title, gpgme_data_t dh)
{
#define BUF_SIZE 5000
    char buf[BUF_SIZE + 1];
    int ret;
    gpg_error_t err;

    ret = gpgme_data_seek(dh, 0, SEEK_SET);
    if (ret) {
        err = gpgme_err_code_from_errno(errno);
        if (err) {
            fprintf(stderr, "data seek failed: <%s> %s\n", gpgme_strsource(err),
                    gpgme_strerror(err));
            exit(1);
        }
    }
    while ((ret = gpgme_data_read(dh, buf, BUF_SIZE)) > 0) {
        //fwrite(buf, ret, 1, stdout);
        printkey(title, buf, ret);
    }
    if (ret < 0) {
        err = gpgme_err_code_from_errno(errno);
        if (err) {
            fprintf(stderr, "data read failed: <%s> %s\n", gpgme_strsource(err),
                    gpgme_strerror(err));
            exit(1);
        }
    }
}

int main(int argc, char **argv)
{
    int showkeyflag = 0, showsecflag = 0, decodeflag = 0;
    int encodeflag = 0, encryptflag = 0, decryptflag = 0;

    if (argc < 2 || argv[1][0] != '-')
        goto usage;
    for (--argc, ++argv; argc > 0 && argv[0][0] == '-'; --argc, ++argv)
        switch (argv[0][1]) {
        case 'K':
            ++showsecflag;
            showkeyflag = 0;
            continue;
        case 'k':
            ++showkeyflag;
            showsecflag = 0;
            continue;
        case 'e':
            ++encryptflag;
            showkeyflag = 0;
            showsecflag = 0;
            continue;
        case 'd':
            ++decryptflag;
            showkeyflag = 0;
            showsecflag = 0;
            continue;
        case 'E':
            ++encodeflag;
            showkeyflag = 0;
            showsecflag = 0;
            continue;
        case 'D':
            ++decodeflag;
            showkeyflag = 0;
            showsecflag = 0;
            continue;
        default:
        usage:
            fprintf(stderr, "Usage:\n");
            fprintf(stderr, "\tcryptotool -e password < infile > outfile\n");
            fprintf(stderr, "\t\t- encrypt file with password\n");
            fprintf(stderr, "\tcryptotool -d password < infile > outfile\n");
            fprintf(stderr, "\t\t- decrypt file with password\n");
            fprintf(stderr, "\tcryptotool -E user < infile > outfile\n");
            fprintf(stderr, "\t\t- encrypt file using public key\n");
            fprintf(stderr, "\tcryptotool -D user < infile > outfile\n");
            fprintf(stderr, "\t\t- decrypt file using private key\n");
            fprintf(stderr, "\tcryptotool -k user\n");
            fprintf(stderr, "\t\t- show user public key\n");
            fprintf(stderr, "\tcryptotool -K user\n");
            fprintf(stderr, "\t\t- show user secret key\n");
            return 1;
        }

    if (argc != 1)
        goto usage;
    const char *userid = argv[0];

    //
    // Initialize GPG library.
    //
    gpg_error_t err;
    gpgme_check_version(NULL);
    err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
    if (err) {
        fprintf(stderr, "check version failed: <%s> %s\n", gpgme_strsource(err),
                gpgme_strerror(err));
        exit(1);
    }

    gpgme_ctx_t ctx;
    err = gpgme_new(&ctx);
    if (err) {
        fprintf(stderr, "cannot create gpg context: <%s> %s\n", gpgme_strsource(err),
                gpgme_strerror(err));
        exit(1);
    }
    gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);
    gpgme_set_armor(ctx, 1);

    if (encryptflag || decryptflag) {
        cryptfile(argv[0], decryptflag);
        return 0;
    }

    gpgme_key_t keyarray[2];
    if (!decodeflag && !showsecflag) {
        err = gpgme_get_key(ctx, userid, &keyarray[0], 0);
        if (err) {
            fprintf(stderr, "error getting key for '%s': <%s> %s\n", userid, gpgme_strsource(err),
                    gpg_strerror(err));
            exit(1);
        }
    }
    if (showkeyflag) {
        // Allocate data buffer,
        gpgme_data_t out;
        err = gpgme_data_new(&out);
        if (err) {
            fprintf(stderr, "cannot allocate data: <%s> %s\n", gpgme_strsource(err),
                    gpgme_strerror(err));
            exit(1);
        }

        // Extract key.
        gpgme_export_mode_t mode = 0;
        err = gpgme_op_export_keys(ctx, keyarray, mode, out);
        if (err) {
            fprintf(stderr, "cannot export keys: <%s> %s\n", gpgme_strsource(err),
                    gpgme_strerror(err));
            exit(1);
        }

        // Print the key.
        fprintf(stderr, "Public key: %s\n", userid);
        print_data("key", out);
        gpgme_data_release(out);
    }
    if (showsecflag || decodeflag) {
#if 0
        /* Get password. */
        struct MD5Context mdContext;
        char *pass = getpass("Password: ");

        /* Calculate the hash */
        MD5Init(&mdContext);
        MD5Update(&mdContext, (unsigned char *)pass, strlen(pass));
        memset(pass, 0, strlen(pass));
        MD5Final((unsigned char *)passwd, &mdContext);

        strcpy(userid, id);
        err = getsecretkey((unsigned char *)passwd, crypt_secfile, userid, sn, se, sd, sp, sq, su);
#else
        err = -1;
#endif
        if (err < 0) {
            fprintf(stderr, "getsecretkey error %d\n", err);
            exit(1);
        }
    }

    if (showkeyflag) {
        ;
    } else if (encodeflag) {
        // TODO
    } else if (showsecflag) {
#if 0
        fprintf(stderr, "Secret key: %.*s\n", userid[0], userid + 1);
        printkey("syse", se);
        printkey("sysn", sn);
        printkey("sysd", sd);
        printkey("sysp", sp);
        printkey("sysq", sq);
        printkey("sysu", su);
#else
        fprintf(stderr, "TODO\n");
#endif
    } else if (decodeflag) {
        decode();
    } else {
        encode();
    }

    // Cleanup.
    gpgme_key_unref(keyarray[0]);
    gpgme_release(ctx);
    return 0;
}
