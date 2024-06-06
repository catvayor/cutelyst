/*
 * SPDX-FileCopyrightText: (C) 2017-2023 Matthias Fehring <mf@huessenbergnetz.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STATICCOMPRESSED_P_H
#define STATICCOMPRESSED_P_H

#include "staticcompressed.h"

#include <QDir>
#include <QRegularExpression>
#include <QVector>

#ifdef CUTELYST_STATICCOMPRESSED_WITH_ZSTD
#    include <zstd.h>
#endif

namespace Cutelyst {

class Context;

class StaticCompressedPrivate
{
public:
    enum Compression { Gzip, Zopfli, Brotli, Deflate, Zstd };

    void beforePrepareAction(Context *c, bool *skipMethod);
    bool locateCompressedFile(Context *c, const QString &relPath) const;
    [[nodiscard]] QString locateCacheFile(const QString &origPath,
                                          const QDateTime &origLastModified,
                                          Compression compression) const;
    [[nodiscard]] bool compressGzip(const QString &inputPath,
                                    const QString &outputPath,
                                    const QDateTime &origLastModified) const;
    [[nodiscard]] bool compressDeflate(const QString &inputPath, const QString &outputPath) const;
#ifdef CUTELYST_STATICCOMPRESSED_WITH_ZOPFLI
    [[nodiscard]] bool compressZopfli(const QString &inputPath, const QString &outputPath) const;
#endif

#ifdef CUTELYST_STATICCOMPRESSED_WITH_BROTLI
    void loadBrotliConfig(const QVariantMap &conf);

    [[nodiscard]] bool compressBrotli(const QString &inputPath, const QString &outputPath) const;

    struct BrotliConfig {
        constexpr static int qualityLevelDefault{11};
        int qualityLevel{qualityLevelDefault};
    } brotli;
#endif

#ifdef CUTELYST_STATICCOMPRESSED_WITH_ZSTD
    [[nodiscard]] bool loadZstdConfig(const QVariantMap &conf);

    [[nodiscard]] bool compressZstd(const QString &inputPath, const QString &outputPath) const;

    struct ZstdConfig {
        ~ZstdConfig() { ZSTD_freeCCtx(ctx); }

        ZSTD_CCtx *ctx{nullptr};
        constexpr static int compressionLevelDefault{9};
        int compressionLevel{compressionLevelDefault};
    } zstd;
#endif

    QVariantMap defaultConfig;
    QStringList dirs;
    QStringList mimeTypes;
    QStringList suffixes;
    QVector<QDir> includePaths;
    QRegularExpression re = QRegularExpression(QStringLiteral("\\.[^/]+$"));
    QDir cacheDir;
    constexpr static int zlibCompressionLevelDefault{9};
    constexpr static int zlibCompressionLevelMin{0};
    constexpr static int zlibCompressionLevelMax{9};
    int zlibCompressionLevel{zlibCompressionLevelDefault};
    constexpr static int zopfliIterationsDefault{15};
    constexpr static int zopfliIterationsMin{1};
    int zopfliIterations{zopfliIterationsDefault};
    bool useZopfli{false};
    bool checkPreCompressed{true};
    bool onTheFlyCompression{true};
    bool serveDirsOnly{false};
};

} // namespace Cutelyst

#endif // STATICCOMPRESSED_P_H
