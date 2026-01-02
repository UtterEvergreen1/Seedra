#pragma once

#include <list>
#include <unordered_map>
#include <utility>

#include "common/nbt.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/RotationAndMirror.hpp"
#include "components/BoundingBox.hpp"
#include "lce/blocks/blocksInit.hpp"

class PlacementSettings;
class World;


class Template {
public:

    class BlockInfo {
    public:
        Pos3D pos;
        lce::BlockState blockState;
        NBTCompound tileEntityData;

        BlockInfo(const Pos3D posIn, const lce::BlockState stateIn, NBTCompound compoundIn)
            : pos(posIn), blockState(stateIn), tileEntityData(std::move(compoundIn)) {}
    };

    class EntityInfo {
    public:
        Pos3DTemplate<double> pos;
        Pos3D blockPos;
        NBTCompound entityData;

        EntityInfo(const Pos3DTemplate<double>& vecIn, const Pos3D posIn, NBTCompound compoundIn)
            : pos(vecIn), blockPos(posIn), entityData(std::move(compoundIn)) {}
    };

    class ITemplateProcessor {
    protected:
        MU Pos3D m_pos{};
        MU const PlacementSettings& m_settings;
    public:
        ITemplateProcessor() = delete;

        ITemplateProcessor(Pos3D posIn, const PlacementSettings& settings)
            : m_pos(posIn), m_settings(settings) {}

        MU virtual std::optional<BlockInfo> processBlock(World& world, Pos3D blockPos, BlockInfo template$blockInfo) {
            return template$blockInfo;
        };

    };


    class BlockRotationProcessor : public ITemplateProcessor {
    public:

        BlockRotationProcessor() = delete;

        BlockRotationProcessor(Pos3D posIn, const PlacementSettings& settingsIn)
            : ITemplateProcessor(posIn, settingsIn) {}

        std::optional<BlockInfo> processBlock(World& world, Pos3D blockPos, BlockInfo template$blockInfo) override {
            return template$blockInfo;
        }
    };


private:
    std::list<BlockInfo> blocks = std::list<BlockInfo>();
    std::list<EntityInfo> entities = std::list<EntityInfo>();

    Pos3D size = {0, 0, 0};

    std::string author = "?";


public:

    ND Pos3D getSize() const {
        return this->size;
    }

    MU void setAuthor(const std::string& authorIn) {
        this->author = authorIn;
    }

    MU ND std::string getAuthor() {
        return this->author;
    }

    /*
    void takeBlocksFromWorld(World worldIn, const Pos3D startPos, const Pos3D endPos, const bool takeEntities, lce::BlockState toIgnore) {
        if (endPos.getX() >= 1 && endPos.getY() >= 1 && endPos.getZ() >= 1) {
            const Pos3D blockPos = startPos.add(endPos).add(-1, -1, -1);
            std::list<BlockInfo> list = std::list<BlockInfo>();
            std::list<BlockInfo> list1 = std::list<BlockInfo>();
            std::list<BlockInfo> list2 = std::list<BlockInfo>();
            Pos3D blockPos1 =
                    Pos3D(std::min(startPos.getX(), blockPos.getX()), std::min(startPos.getY(), blockPos.getY()),
                                 std::min(startPos.getZ(), blockPos.getZ()));
            Pos3D blockPos2 =
                    Pos3D(std::max(startPos.getX(), blockPos.getX()), std::max(startPos.getY(), blockPos.getY()),
                                 std::max(startPos.getZ(), blockPos.getZ()));
            this->size = endPos;

            for (Pos3D blockPos$mutableBlockPos: BlockPos::getAllInBoxMutable(blockPos1, blockPos2)) {
                Pos3D blockpos3 = blockPos$mutableBlockPos.subtract(blockPos1);
                lce::BlockState iBlockState = worldIn.getBlockState(blockPos$mutableBlockPos);

                if (toIgnore == null || toIgnore != iBlockState.getBlock()) {
                    TileEntity tileentity = worldIn.getTileEntity(blockPos$mutableBlockPos);

                    if (tileentity != null) {
                        NBTCompound nBTTagCompound = tileentity.writeToNBT(new NBTCompound());
                        nBTTagCompound.removeTag("x");
                        nBTTagCompound.removeTag("y");
                        nBTTagCompound.removeTag("z");
                        list1.add(new Template::BlockInfo(blockpos3, iBlockState, nBTTagCompound));
                    } else if (!iBlockState.isFullBlock() && !iBlockState.isFullCube()) {
                        list2.add(new Template::BlockInfo(blockpos3, iBlockState, (NBTCompound) null));
                    } else {
                        list.add(new Template::BlockInfo(blockpos3, iBlockState, (NBTCompound) null));
                    }
                }
            }

            this->blocks.clear();
            this->blocks.addAll(list);
            this->blocks.addAll(list1);
            this->blocks.addAll(list2);

            if (takeEntities) {
                this->takeEntitiesFromWorld(worldIn, blockPos1, blockPos2.add(1, 1, 1));
            } else {
                this->entities.clear();
            }
        }
    }
    */

private:
    /*
    void takeEntitiesFromWorld(World& worldIn, Pos3D startPos, Pos3D endPos) {
        std::list<Entity> list = worldIn.<Entity>getEntitiesWithinAABB(
                Entity.class, new AxisAlignedBB(startPos, endPos), new Predicate<Entity>() {
                public
                    bool apply(@Nullable Entity entityCandidate) {
                        return !(entityCandidate instanceof EntityPlayer);
                    }
                });
        this->entities.clear();

        for (Entity entity: list) {
            Pos3DTemplate<double> vec3d = new Pos3DTemplate<double>(entity.posX - (double) startPos.getX(), entity.posY - (double) startPos.getY(),
                                    entity.posZ - (double) startPos.getZ());
            NBTCompound nBTTagCompound = new NBTCompound();
            entity.writeToNBTOptional(nBTTagCompound);
            Pos3D blockPos;

            if (entity instanceof EntityPainting) {
                blockPos = ((EntityPainting) entity).getHangingPosition().subtract(startPos);
            } else {
                blockPos = Pos3D(vec3d);
            }

            this->entities.add(new Template::EntityInfo(vec3d, blockPos, nBTTagCompound));
        }
    }
    */

public:
    std::map<Pos3D, std::string> getDataBlocks(Pos3D pos, PlacementSettings& placementIn);



    MU static Pos3D calculateConnectedPos(const PlacementSettings& fromSettings, Pos3D fromPos, const PlacementSettings& toSettings, Pos3D toPos);

    static Pos3D transformedBlockPos(const PlacementSettings& settings, Pos3D localPos);

    MU void addBlocksToWorldChunk(World& worldIn, Pos3D pos, PlacementSettings& placementIn);

    void addBlocksToWorld(World& worldIn, Pos3D pos, PlacementSettings& placementIn);

    void addBlocksToWorld(World& worldIn, Pos3D pos, PlacementSettings& placementIn, int flags);

    void addBlocksToWorld(World& world, Pos3D origin, MU ITemplateProcessor* processor, PlacementSettings& settings, MU int flags);

private:

    MU void addEntitiesToWorld(MU World& worldIn, Pos3D pos, Mirror mirrorIn, Rotation rotationIn, BoundingBox aabb);
public:
    ND Pos3D transformedSize(Rotation rotationIn) const;
private:
    static Pos3D transformedBlockPos(Pos3D pos, Mirror mirrorIn, Rotation rotationIn);

    MU static Pos3DTemplate<double> transformedBlockPos(Pos3DTemplate<double> vec, Mirror mirrorIn, Rotation rotationIn);
public:
    MU ND Pos3D getZeroPositionWithTransform(Pos3D origin, Mirror mirror, Rotation rotation) const;

    static Pos3D transformOrigin(Pos3D origin, Mirror mirror, Rotation rotation, int width, int depth);

    MU static void registerTemplateDataFixers(/*DataFixer dataFixer*/);

public:
    /*
    NBTCompound writeToNBT(NBTCompound nbt) {
        Template::BasicPalette template$basicPalette = new Template::BasicPalette();
        NBTList nBTTagList = new NBTList();

        for (Template::BlockInfo template$blockInfo: this->blocks) {
            NBTCompound nBTTagCompound = new NBTCompound();
            nBTTagCompound.setTag("pos", this->writeInts(template$blockInfo.pos.getX(), template$blockInfo.pos.getY(),
                                                        template$blockInfo.pos.getZ()));
            nBTTagCompound.setInteger("state", template$basicPalette.idFor(template$blockInfo.blockState));

            if (template$blockInfo.tileEntityData != null) {
                nBTTagCompound.setTag("nbt", template$blockInfo.tileEntityData);
            }

            nBTTagList.appendTag(nBTTagCompound);
        }

        NBTList nBTTagList1 = new NBTList();

        for (Template::EntityInfo template$entityinfo: this->entities) {
            NBTCompound nBTTagCompound1 = new NBTCompound();
            nBTTagCompound1.setTag("pos", this->writeDoubles(template$entityinfo.pos.xCoord, template$entityinfo.pos.yCoord,
                                                            template$entityinfo.pos.zCoord));
            nBTTagCompound1.setTag("blockPos",
                                   this->writeInts(template$entityinfo.blockPos.getX(), template$entityinfo.blockPos.getY(),
                                                  template$entityinfo.blockPos.getZ()));

            if (template$entityinfo.entityData != null) { nBTTagCompound1.setTag("nbt", template$entityinfo.entityData); }

            nBTTagList1.appendTag(nBTTagCompound1);
        }

        NBTList nBTTagList2 = new NBTList();

        for (lce::BlockState iBlockState: template$basicPalette) {
            nBTTagList2.appendTag(NBTUtil.writeBlockState(new NBTCompound(), iBlockState));
        }

        nbt.setTag("palette", nBTTagList2);
        nbt.setTag("blocks", nBTTagList);
        nbt.setTag("entities", nBTTagList1);
        nbt.setTag("size", this->writeInts(this->size.getX(), this->size.getY(), this->size.getZ()));
        nbt.setString("author", this->author);
        nbt.setInteger("DataVersion", 1139);
        return nbt;
    }
    */

    MU static inline bool readPosXYZ(const NBTBase& tag, int& x, int& y, int& z);

    void read(NBTCompound& compound);

private:
    /*
    NBTList writeInts(int... values) {
        NBTList nBTTagList = new NBTList();

        for (int i: values) { nBTTagList.appendTag(new NBTTagInt(i)); }

        return nBTTagList;
    }

    NBTList writeDoubles(double... values) {
        NBTList nBTTagList = new NBTList();

        for (double d0: values) { nBTTagList.appendTag(new NBTDouble(d0)); }

        return nBTTagList;
    }
    */

    class BasicPalette {
    public:
        static inline const lce::BlockState DEFAULT_BLOCK_STATE = lce::BlocksInit::AIR.getDefaultState();

    private:
        struct BlockStateHash {
            size_t operator()(const lce::BlockState& state) const noexcept {
                return std::hash<int>{}(state.getID() + 16 * state.getDataTag());
            }
        };

        struct BlockStateEqual {
            bool operator()(const lce::BlockState& a, const lce::BlockState& b) const noexcept {
                return a == b;
            }
        };

        std::unordered_map<lce::BlockState, int, BlockStateHash, BlockStateEqual> m_stateToId;
        std::unordered_map<int, lce::BlockState> m_idToState;

    public:
        BasicPalette() = default;

        MU int idFor(const lce::BlockState& state) {
            auto it = m_stateToId.find(state);

            if (it == m_stateToId.end()) {
                const int newId = static_cast<int>(m_stateToId.size());
                m_stateToId.emplace(state, newId);
                m_idToState.emplace(newId, state);
                return newId;
            }

            return it->second;
        }

        ND lce::BlockState stateFor(const int id) const {
            const auto it = m_idToState.find(id);
            return (it == m_idToState.end()) ? DEFAULT_BLOCK_STATE : it->second;
        }

        auto begin() { return m_stateToId.begin(); }
        auto end() { return m_stateToId.end(); }
        ND auto begin() const { return m_stateToId.begin(); }
        ND auto end() const { return m_stateToId.end(); }

        void addMapping(const lce::BlockState& state, int id) {
            m_stateToId.emplace(state, id);
            m_idToState.emplace(id, state);
        }

        ND size_t size() const { return m_stateToId.size(); }

        ND bool contains(const lce::BlockState& state) const {
            return m_stateToId.contains(state);
        }

        MU ND bool containsId(const int id) const {
            return m_idToState.contains(id);
        }
    };



};
