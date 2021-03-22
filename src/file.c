﻿/* -*- coding: utf-8-with-signature; tab-width: 8; indent-tabs-mode: t; -*- */

/*
 * Suika 2
 * Copyright (C) 2001-2021, TABATA Keiichi. All rights reserved.
 */

/*
 * [Changes]
 *  - 2016/06/28 作成
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "suika.h"

/* パッケージ内のファイルエントリの最大数 */
#define ENTRY_SIZE		(65536)

/* パッケージ内のファイル名のサイズ */
#define FILE_NAME_SIZE		(256)

/* ファイル読み込みストリーム */
struct rfile {
	/* パッケージ内のファイルであるか */
	bool is_packaged;

	/* パッケージファイルもしくは個別のファイルへのファイルポインタ */
	FILE *fp;

	/* パッケージ内のファイルを使う場合にのみ用いる情報 */
	uint64_t size;
	uint64_t offset;
	uint64_t pos;
};

/* ファイル書き込みストリーム (TODO: 難読化をサポートする) */
struct wfile {
	FILE *fp;
};

/* パッケージのファイルエントリ */
static struct file_entry {
	char name[FILE_NAME_SIZE];
	uint64_t size;
	uint64_t offset;
} entry[ENTRY_SIZE];

/* パッケージのファイルエントリ数 */
static uint64_t entry_count;

/* パッケージファイルのパス */
static char *package_path;

/*
 * 前方参照
 */
static bool check_file_name(const char *file);
static void ungetc_rfile(struct rfile *rf, char c);

/*
 * 初期化
 */

/*
 * ファイル読み込みの初期化処理を行う
 */
bool init_file(void)
{
	FILE *fp;
	uint64_t i;

	/* パッケージファイルのパスを求める */
	package_path = make_valid_path(NULL, PACKAGE_FILE);
	if (package_path == NULL)
		return false;

	/* パッケージファイルを開いてみる */
	fp = fopen(package_path, "rb");
	if (fp == NULL) {
		free(package_path);
		package_path = NULL;
		return true;	/* 開けなくてもよい */
	}

	/* パッケージのファイルエントリ数を取得する */
	if (fread(&entry_count, sizeof(uint64_t), 1, fp) < 1) {
		log_package_file_error();
		fclose(fp);
		return false;
	}
	if (entry_count > ENTRY_SIZE) {
		log_package_file_error();
		fclose(fp);
		return false;
	}

	/* パッケージのファイルエントリを読み込む */
	for (i = 0; i < entry_count; i++) {
		if (fread(&entry[i].name, FILE_NAME_SIZE, 1, fp) < 1)
			break;
		if (fread(&entry[i].size, sizeof(uint64_t), 1, fp) < 1)
			break;
		if (fread(&entry[i].offset, sizeof(uint64_t), 1, fp) < 1)
			break;
	}
	if (i != entry_count) {
		log_package_file_error();
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

/*
 * ファイル読み込みの終了処理を行う
 */
void cleanup_file(void)
{
	free(package_path);
}

/*
 * 読み込み
 */

/*
 * ファイル読み込みストリームを開く
 */
struct rfile *open_rfile(const char *dir, const char *file, bool save_data)
{
	char entry_name[FILE_NAME_SIZE];
	char *real_path;
	struct rfile *rf;
	uint64_t i;

	/* ファイル名に半角英数字以外が含まれるかチェックする */
	if (!check_file_name(file)) {
		log_file_name(dir, file);
		return NULL;
	}

	/* rfile構造体のメモリを確保する */
	rf = malloc(sizeof(struct rfile));
	if (rf == NULL) {
		log_memory();
		return NULL;
	}

	/* パスを生成する */
	real_path = make_valid_path(dir, file);
	if (real_path == NULL) {
		log_memory();
		free(rf);
		return NULL;
	}

	/* まずファイルシステム上のファイルを開いてみる */
	rf->fp = fopen(real_path, "rb");
	if (rf->fp != NULL) {
		/* 開けた場合、ファイルシステム上のファイルを用いる */
		free(real_path);
		rf->is_packaged = false;
		return rf;
	}
	free(real_path);

	/* セーブデータはパッケージから探さない */
	if (save_data) {
		free(rf);
		return NULL;
	}

	/* パッケージがなければエラーとする */
	if (package_path == NULL) {
		log_dir_file_open(dir, file);
		free(rf);
		return NULL;
	}

	/* 次にパッケージ上のファイルエントリを探す(TODO: sort and search) */
	snprintf(entry_name, FILE_NAME_SIZE, "%s/%s", dir, file);
	for (i = 0; i < entry_count; i++) {
		if (strcmp(entry[i].name, entry_name) == 0)
			break;
	}
	if (i == entry_count) {
		/* みつからなかった場合 */
		log_dir_file_open(dir, file);
		free(rf);
		return NULL;
	}

	/* みつかった場合、パッケージファイルを別なファイルポインタで開く */
	rf->fp = fopen(package_path, "rb");
	if (rf->fp == NULL) {
		log_file_open(PACKAGE_FILE);
		free(rf);
		return NULL;
	}

	/* 読み込み位置にシークする */
	if (fseek(rf->fp, (long)entry[i].offset, SEEK_SET) != 0) {
		log_package_file_error();
		free(rf);
		return 0;
	}

	rf->is_packaged = true;
	rf->size = entry[i].size;
	rf->offset = entry[i].offset;
	rf->pos = 0;

	return rf;
}

/* ファイル名に半角英数字以外が含まれるかチェックする */
static bool check_file_name(const char *file)
{
	const char *c;

	c = file;
	while (*c) {
		/* UTF-8文字列に半角英数字以外が含まれる場合 */
		if (*c & 0x80)
			return false;
		c++;
	}

	return true;
}

/*
 * ファイルのサイズを取得する
 */
size_t get_rfile_size(struct rfile *rf)
{
	long pos, len;

	/* ファイルシステム上のファイルの場合 */
	if (!rf->is_packaged) {
		pos = ftell(rf->fp);
		fseek(rf->fp, 0, SEEK_END);
		len = ftell(rf->fp);
		fseek(rf->fp, pos, SEEK_SET);
		return (size_t)len;
	}

	/* パッケージ内のファイルの場合 */
	return (size_t)rf->size;
}

/*
 * ファイル読み込みストリームから読み込む
 */
size_t read_rfile(struct rfile *rf, void *buf, size_t size)
{
	size_t len;

	assert(rf != NULL);
	assert(rf->fp != NULL);

	/* ファイルシステム上のファイルの場合 */
	if (!rf->is_packaged) {
		len = fread(buf, 1, size, rf->fp);
		return len;
	}

	/* パッケージ内のファイルの場合 */
	if (rf->pos + size > rf->size)
		size = (size_t)(rf->size - rf->pos);
	if (size == 0)
		return 0;
	len = fread(buf, 1, size, rf->fp);
	rf->pos += len;
	return len;
}

/* ファイル読み込みストリームに1文字戻す */
static void ungetc_rfile(struct rfile *rf, char c)
{
	assert(rf != NULL);
	assert(rf->fp != NULL);

	if (rf->fp != NULL) {
		/* ファイルシステム上のファイルの場合 */
		ungetc(c, rf->fp);
	} else {
		/* パッケージ内のファイルの場合 */
		assert(rf->pos != 0);
		ungetc(c, rf->fp);
		rf->pos--;
	}
}

/*
 * ファイル読み込みストリームから1行読み込む
 */
const char *gets_rfile(struct rfile *rf, char *buf, size_t size)
{
	char *ptr;
	size_t len;
	char c;

	assert(rf != NULL);
	assert(rf->fp != NULL);

	ptr = buf;

	for (len = 0; len < size - 1; len++) {
		if (read_rfile(rf, &c, 1) != 1) {
			*ptr = '\0';
			return len == 0 ? NULL : buf;
		}
		if (c == '\n' || c == '\0') {
			*ptr = '\0';
			return buf;
		}
		if (c == '\r') {
			if (read_rfile(rf, &c, 1) != 1) {
				ungetc_rfile(rf, c);
				*ptr = '\0';
				return buf;
			}
			if (c == '\n') {
				*ptr = '\0';
				return buf;
			}
			ungetc_rfile(rf, c);
			*ptr = '\0';
			return buf;
		}
		*ptr++ = c;
	}
	*ptr = '\0';
	return buf;
}

/*
 * ファイル読み込みストリームを閉じる
 */
void close_rfile(struct rfile *rf)
{
	assert(rf != NULL);
	assert(rf->fp != NULL);

	fclose(rf->fp);
	free(rf);
}

/*
 * 書き込み
 */

/*
 * ファイル書き込みストリームを開く
 */
struct wfile *open_wfile(const char *dir, const char *file)
{
	char *path;
	struct wfile *wf;

	/* wfile構造体のメモリを確保する */
	wf = malloc(sizeof(struct wfile));
	if (wf == NULL) {
		log_memory();
		return NULL;
	}

	/* パスを生成する */
	path = make_valid_path(dir, file);
	if (path == NULL) {
		log_memory();
		free(wf);
		return NULL;
	}

	/* ファイルをオープンする */
	wf->fp = fopen(path, "wb");
	if (wf->fp == NULL) {
		log_file_open(path);
		free(path);
		free(wf);
		return NULL;
	}
	free(path);

	return wf;
}

/*
 * ファイル書き込みストリームに書き込む
 */
size_t write_wfile(struct wfile *wf, const void *buf, size_t size)
{
	size_t len;

	assert(wf != NULL);
	assert(wf->fp != NULL);

	len = fwrite(buf, 1, size, wf->fp);

	return len;
}

/*
 * ファイル読み込みストリームを閉じる
 */
void close_wfile(struct wfile *wf)
{
	assert(wf != NULL);
	assert(wf->fp != NULL);

	fflush(wf->fp);
	fclose(wf->fp);
	free(wf);
}
